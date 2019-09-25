/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include <cstdint>
#include <exception>
#include <istream>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "common/converter.h"
#include "common/crypto/manager.h"
#include "common/crypto/types.h"
#include "hub/commands/factory.h"
#include "hub/commands/helper.h"
#include "hub/db/helper.h"

#include "hub/commands/user_withdraw.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<UserWithdraw> registrator;

boost::property_tree::ptree UserWithdraw::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  UserWithdrawRequest req;
  UserWithdrawReply rep;

  auto maybeUserId = request.get_optional<std::string>("userId");
  if (!maybeUserId) {
    tree.add("error",
             common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
    return tree;
  }

  req.userId = maybeUserId.value();
  req.validateChecksum = false;
  auto maybeValidateChecksum =
      request.get_optional<std::string>("validateChecksum");
  if (maybeValidateChecksum) {
    req.validateChecksum = common::stringToBool(maybeValidateChecksum.value());
  }

  auto maybePayoutAddress = request.get_optional<std::string>("payoutAddress");
  if (maybePayoutAddress) {
    req.payoutAddress = maybePayoutAddress.value();
  }

  auto maybeAmount = request.get_optional<std::string>("amount");
  if (maybeAmount) {
    std::istringstream iss(maybeAmount.value());
    iss >> req.amount;
  }

  auto maybeTag = request.get_optional<std::string>("tag");
  if (maybeTag) {
    req.tag = maybeTag.value();
  }

  auto status = doProcess(&req, &rep);

  if (status != common::cmd::OK) {
    tree.add("error", common::cmd::getErrorString(status));
  } else {
    tree.add("uuid", rep.uuid);
  }
  return tree;
}

common::cmd::Error UserWithdraw::doProcess(
    const UserWithdrawRequest* request, UserWithdrawReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();

  nonstd::optional<common::cmd::Error> errorCode;
  uint64_t userId;
  uint64_t withdrawalId;

  nonstd::optional<boost::uuids::uuid> withdrawalUUIDOptional;
  try {
    withdrawalUUIDOptional = boost::uuids::random_generator()();
  } catch (const std::exception& ex) {
    LOG(FATAL) << " Failed in generating boost UUID: " << ex.what();
  }
  auto withdrawalUUID = *withdrawalUUIDOptional;

  if (request->amount <= 0) {
    return common::cmd::UNKNOWN_ERROR;
  }

  nonstd::optional<common::crypto::Address> address;
  if (request->validateChecksum) {
    address = std::move(
        common::crypto::CryptoManager::get().provider().verifyAndStripChecksum(
            request->payoutAddress));

    if (!address.has_value()) {
      return common::cmd::INVALID_CHECKSUM;
    }
  } else {
    try {
      address = {common::crypto::Address(request->payoutAddress)};
    } catch (const std::exception& ex) {
      LOG(ERROR) << session()
                 << " Withdrawal to invalid address: " << ex.what();

      return common::cmd::UNKNOWN_ERROR;
    }
  }

  if (!isAddressValid(address->str_view())) {
    return common::cmd::INVALID_ADDRESS;
  }

  auto transaction = connection.transaction();

  try {
    common::crypto::Tag withdrawalTag(
        request->tag +
        std::string(common::crypto::Tag::length() - request->tag.size(), '9'));

    // Get userId for identifier
    {
      auto maybeUserId = connection.userIdFromIdentifier(request->userId);
      if (!maybeUserId) {
        errorCode = common::cmd::USER_DOES_NOT_EXIST;
        goto cleanup;
      }

      userId = maybeUserId.value();
    }

    // Get available balance for user
    {
      auto balance = connection.availableBalanceForUser(userId);

      if (balance < request->amount) {
        errorCode = common::cmd::INSUFFICIENT_BALANCE;
        goto cleanup;
      }
    }

    // Verify address wasn't spent before
    if (_api) {
      auto res = _api->wereAddressesSpentFrom({address.value().str()});
      if (!res.has_value() || res.value().states.empty()) {
        errorCode = common::cmd::IOTA_NODE_UNAVAILABLE;
        goto cleanup;
      } else if (res.value().states.front()) {
        errorCode = common::cmd::ADDRESS_WAS_SPENT;
        goto cleanup;
      }
    }

    // Add withdrawal
    withdrawalId = connection.createWithdrawal(
        boost::uuids::to_string(withdrawalUUID), userId, request->amount,
        withdrawalTag, address.value());

    // Add user account balance entry
    connection.createUserAccountBalanceEntry(
        userId, -request->amount, db::UserAccountBalanceReason::WITHDRAWAL,
        withdrawalId);

    response->uuid = boost::uuids::to_string(withdrawalUUID);

  cleanup:
    if (errorCode) {
      transaction->rollback();
    } else {
      transaction->commit();
    }
  } catch (const std::exception& ex) {
    LOG(ERROR) << session() << " Commit failed: " << ex.what();

    try {
      transaction->rollback();
    } catch (const std::exception& ex) {
      LOG(ERROR) << session() << " Rollback failed: " << ex.what();
    }

    errorCode = common::cmd::UNKNOWN_ERROR;
  }

  if (errorCode) {
    return errorCode.value();
  }

  return common::cmd::OK;
}

}  // namespace cmd
}  // namespace hub
