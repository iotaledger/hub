/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include <algorithm>
#include <cstdint>
#include <set>
#include <unordered_map>

#include <boost/algorithm/string.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "common/converter.h"
#include "hub/db/helper.h"

#include "hub/commands/converter.h"
#include "hub/commands/factory.h"
#include "hub/commands/helper.h"

#include "hub/commands/process_transfer_batch.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<ProcessTransferBatch> registrator;

boost::property_tree::ptree ProcessTransferBatch::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;

  ProcessTransferBatchRequest req;
  ProcessTransferBatchReply rep;

  std::map<std::string, int64_t> userToAmount;
  auto transfers = request.get_child_optional("transfers");

  if (!transfers) {
    tree.add("error",
             common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
    return tree;
  }

  for (auto& transfer : transfers.value()) {
    auto currUser = transfer.second.get_optional<std::string>("userId");
    auto currAmount = transfer.second.get_optional<int64_t>("amount");

    if (!currUser || !currAmount) {
      tree.add("error",
               common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
      return tree;
    }

    userToAmount[currUser.value()] += currAmount.value();
  }

  for (auto&& userAmountPair : userToAmount) {
    req.transfers.emplace_back(cmd::UserTransfer{
        .userId = userAmountPair.first, .amount = userAmountPair.second});
  }

  auto status = doProcess(&req, &rep);

  if (status != common::cmd::OK) {
    tree.add("error", common::cmd::getErrorString(status));
  }

  return tree;
}

common::cmd::Error ProcessTransferBatch::doProcess(
    const ProcessTransferBatchRequest* request,
    ProcessTransferBatchReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();
  std::set<std::string> identifiers;

  for (auto transfer : request->transfers) {
    identifiers.insert(transfer.userId);
  }

  auto identifierToId = connection.userIdsFromIdentifiers(identifiers);
  if (identifierToId.size() < identifiers.size()) {
    return common::cmd::USER_DOES_NOT_EXIST;
  }

  auto validationStatus = validateTransfers(request, identifierToId);
  if (validationStatus != common::cmd::OK) {
    return validationStatus;
  }

  std::vector<hub::db::UserTransfer> transfers;
  for (auto transfer : request->transfers) {
    transfers.emplace_back(hub::db::UserTransfer{
      userId : static_cast<uint64_t>(identifierToId[transfer.userId]),
      amount : transfer.amount
    });
  }

  // Actual transfer insertion to db
  auto transaction = connection.transaction();
  try {
    connection.insertUserTransfers(transfers);
    transaction->commit();
  } catch (sqlpp::exception& ex) {
    LOG(ERROR) << session() << " Commit failed: " << ex.what();

    try {
      transaction->rollback();
    } catch (const sqlpp::exception& ex) {
      LOG(ERROR) << session() << " Rollback failed: " << ex.what();
    }

    return common::cmd::UNKNOWN_ERROR;
  }

  return common::cmd::OK;
}

common::cmd::Error ProcessTransferBatch::validateTransfers(
    const ProcessTransferBatchRequest* request,
    const std::map<std::string, int64_t>& identifierToId) noexcept {
  auto& connection = db::DBManager::get().connection();
  std::unordered_map<std::string, int64_t> userToTransferAmount;

  uint64_t totalBatchSum = 0;

  bool zeroAmount = false;
  // validate amounts are not zero
  for (auto transfer : request->transfers) {
    if (transfer.amount == 0) {
      zeroAmount = true;
      break;
    }
    totalBatchSum += transfer.amount;
    if (userToTransferAmount.find(transfer.userId) ==
        userToTransferAmount.end()) {
      userToTransferAmount[transfer.userId] = 0;
    }
    userToTransferAmount[transfer.userId] += transfer.amount;
  }

  if (zeroAmount) {
    return common::cmd::BATCH_INVALID;
  }
  if (totalBatchSum != 0) {
    return common::cmd::BATCH_AMOUNT_NOT_ZERO;
  }

  std::set<uint64_t> userIds;
  boost::copy(identifierToId | boost::adaptors::map_values,
              std::inserter(userIds, userIds.begin()));
  auto userToBalance = connection.getTotalAmountForUsers(userIds);
  for (auto& kv : userToTransferAmount) {
    uint64_t currId = identifierToId.at(kv.first);

    auto availableBalance = userToBalance.at(currId);
    auto balanceChange = kv.second;

    if ((availableBalance + balanceChange) < 0) {
      return common::cmd::BATCH_INCONSISTENT;
    }
  }

  return common::cmd::OK;
}

}  // namespace cmd

}  // namespace hub
