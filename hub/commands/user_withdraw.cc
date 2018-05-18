// Copyright 2018 IOTA Foundation

#include "hub/commands/user_withdraw.h"

#include <cstdint>

#include <sqlpp11/connection.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/stats/session.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/helper.h"
#include "hub/crypto/types.h"

namespace hub {
namespace cmd {

grpc::Status UserWithdraw::doProcess(
    const hub::rpc::UserWithdrawRequest* request,
    hub::rpc::UserWithdrawReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();
  auto transaction = connection.transaction();

  std::optional<hub::rpc::ErrorCode> errorCode;
  uint64_t userId;
  uint64_t balance;
  uint64_t withdrawalId;

  auto withdrawalUUID = boost::uuids::random_generator()();
  auto payoutAddress = hub::crypto::Address(request->payoutaddress());

  try {
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
      auto maybeBalance = connection.availableBalanceForUser(userId);
      if (!maybeBalance) {
        errorCode = hub::rpc::ErrorCode::EC_UNKNOWN;
        goto cleanup;
      }

      balance = maybeBalance.value();
    }

    if (balance < request->amount()) {
      errorCode = hub::rpc::ErrorCode::INSUFFICIENT_BALANCE;
      goto cleanup;
    }

    // Add withdrawal
    withdrawalId =
        connection.createWithdrawal(boost::uuids::to_string(withdrawalUUID),
                                    userId, request->amount(), payoutAddress);

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
  } catch (sqlpp::exception& ex) {
    LOG(ERROR) << session() << " Commit failed: " << ex.what();

    try {
      transaction->rollback();
    } catch (const sqlpp::exception& ex) {
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
