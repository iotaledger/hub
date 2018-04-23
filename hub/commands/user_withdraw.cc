#include "user_withdraw.h"

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

#include "helper.h"

using namespace sqlpp;

namespace hub {
namespace cmd {

grpc::Status UserWithdraw::doProcess(
    const hub::rpc::UserWithdrawRequest* request,
    hub::rpc::UserWithdrawReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();
  transaction_t<hub::db::Connection> transaction(connection, true);

  std::optional<hub::rpc::ErrorCode> errorCode;
  uint64_t userId;
  uint64_t balance;
  uint64_t withdrawalId;
  auto withdrawalUUID = boost::uuids::random_generator()();

  try {
    // Get userId for identifier
    {
      auto maybeUserId =
          db::userIdFromIdentifier(connection, request->userid());
      if (!maybeUserId) {
        errorCode = hub::rpc::ErrorCode::USER_DOES_NOT_EXIST;
        goto cleanup;
      }

      userId = maybeUserId.value();
    }

    // Get available balance for user
    {
      auto maybeBalance = db::availableBalanceForUser(connection, userId);
      if (!maybeBalance) {
        errorCode = hub::rpc::ErrorCode::UNKNOWN;
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
        db::createWithdrawal(connection, withdrawalUUID, userId,
                             request->amount(), request->payoutaddress());

    // Add user account balance entry
    db::createUserAccountBalanceEntry(connection, userId, request->amount(),
                                      db::UserAccountBalanceReason::WITHDRAWAL,
                                      withdrawalId);

    response->set_uuid(boost::uuids::to_string(withdrawalUUID));

  cleanup:
    if (errorCode) {
      transaction.rollback();
    } else {
      transaction.commit();
    }
  } catch (sqlpp::exception& ex) {
    LOG(ERROR) << session() << " Commit failed: " << ex.what();
    errorCode = hub::rpc::ErrorCode::UNKNOWN;
  }

  if (errorCode) {
    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(errorCode.value()));
  }

  return grpc::Status::CANCELLED;
}

}  // namespace cmd
}  // namespace hub
