/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/user_withdraw.h"

#include <cstdint>
#include <exception>
#include <utility>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "common/stats/session.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "common/crypto/manager.h"
#include "common/crypto/types.h"
#include "hub/commands/helper.h"

namespace hub {
namespace cmd {

grpc::Status UserWithdraw::doProcess(
    const hub::rpc::UserWithdrawRequest* request,
    hub::rpc::UserWithdrawReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();
  auto transaction = connection.transaction();

  nonstd::optional<hub::rpc::ErrorCode> errorCode;
  uint64_t userId;
  uint64_t withdrawalId;

  auto withdrawalUUID = boost::uuids::random_generator()();

  if (request->amount() <= 0) {
    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(hub::rpc::ErrorCode::EC_UNKNOWN));
  }

  try {
    nonstd::optional<common::crypto::Address> address;
    if (request->validatechecksum()) {
      address =
          std::move(common::crypto::CryptoManager::get()
                        .provider()
                        .verifyAndStripChecksum(request->payoutaddress()));

      if (!address.has_value()) {
        return grpc::Status(
            grpc::StatusCode::FAILED_PRECONDITION, "",
            errorToString(hub::rpc::ErrorCode::CHECKSUM_INVALID));
      }
    } else {
      address = {common::crypto::Address(request->payoutaddress())};
    }

    common::crypto::Tag withdrawalTag(
        request->tag() +
        std::string(common::crypto::Tag::length() - request->tag().size(),
                    '9'));

    // Get userId for identifier
    {
      auto maybeUserId = connection.userIdFromIdentifier(request->userid());
      if (!maybeUserId) {
        errorCode = hub::rpc::ErrorCode::USER_DOES_NOT_EXIST;
        goto cleanup;
      }

      userId = maybeUserId.value();
    }

    // Get available balance for user
    {
      auto balance = connection.availableBalanceForUser(userId);

      if (balance < request->amount()) {
        errorCode = hub::rpc::ErrorCode::INSUFFICIENT_BALANCE;
        goto cleanup;
      }
    }

    // Add withdrawal
    withdrawalId = connection.createWithdrawal(
        boost::uuids::to_string(withdrawalUUID), userId, request->amount(),
        withdrawalTag, address.value());

    // Add user account balance entry
    connection.createUserAccountBalanceEntry(
        userId, -request->amount(), db::UserAccountBalanceReason::WITHDRAWAL,
        withdrawalId);

    response->set_uuid(boost::uuids::to_string(withdrawalUUID));

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

    errorCode = hub::rpc::ErrorCode::EC_UNKNOWN;
  }

  if (errorCode) {
    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(errorCode.value()));
  }

  return grpc::Status::OK;
}

}  // namespace cmd
}  // namespace hub
