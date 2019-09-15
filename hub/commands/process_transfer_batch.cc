/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/process_transfer_batch.h"

#include <algorithm>
#include <cstdint>
#include <set>
#include <unordered_map>
#include <vector>

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm/copy.hpp>

#include <sqlpp11/connection.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

#include "common/stats/session.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/converter.h"
#include "hub/commands/factory.h"
#include "hub/commands/helper.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<ProcessTransferBatch> registrator;

boost::property_tree::ptree ProcessTransferBatch::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  return tree;
}

common::cmd::Error ProcessTransferBatch::doProcess(
    const hub::rpc::ProcessTransferBatchRequest* request,
    hub::rpc::ProcessTransferBatchReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();
  std::set<std::string> identifiers;

  for (auto i = 0; i < request->transfers_size(); ++i) {
    const auto& t = request->transfers(i);
    identifiers.insert(t.userid());
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
  for (auto i = 0; i < request->transfers_size(); ++i) {
    const hub::rpc::ProcessTransferBatchRequest_Transfer& t =
        request->transfers(i);
    transfers.emplace_back(hub::db::UserTransfer{
        static_cast<uint64_t>(identifierToId[t.userid()]), t.amount()});
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
    const hub::rpc::ProcessTransferBatchRequest* request,
    const std::map<std::string, int64_t>& identifierToId) noexcept {
  auto& connection = db::DBManager::get().connection();
  std::unordered_map<std::string, int64_t> userToTransferAmount;

  uint64_t totalBatchSum = 0;

  bool zeroAmount = false;
  // validate amounts are not zero
  for (auto i = 0; i < request->transfers_size(); ++i) {
    const auto& t = request->transfers(i);
    if (t.amount() == 0) {
      zeroAmount = true;
      break;
    }
    totalBatchSum += t.amount();
    if (userToTransferAmount.find(t.userid()) == userToTransferAmount.end()) {
      userToTransferAmount[t.userid()] = 0;
    }
    userToTransferAmount[t.userid()] += t.amount();
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
