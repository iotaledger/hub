// Copyright 2018 IOTA Foundation

#include "hub/commands/user_withdraw_cancel.h"

#include <cstdint>

#include <sqlpp11/connection.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/stats/session.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/helper.h"
#include "hub/crypto/types.h"

namespace hub {
namespace cmd {

grpc::Status UserWithdrawCancel::doProcess(
    const hub::rpc::UserWithdrawCancelRequest* request,
    hub::rpc::UserWithdrawCancelReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();
  sqlpp::transaction_t<hub::db::Connection> transaction(connection, true);

  std::optional<hub::rpc::ErrorCode> errorCode;
  std::optional<bool> success;

  auto uuid = hub::crypto::UUID::generate().toString();

  try {
    auto result = db::cancelWithdrawal(connection, uuid);

    success = result != 0;

  cleanup:
    if (errorCode) {
      transaction.rollback();
    } else {
      transaction.commit();
    }
  } catch (sqlpp::exception& ex) {
    LOG(ERROR) << session() << " Commit failed: " << ex.what();
    errorCode = hub::rpc::ErrorCode::EC_UNKNOWN;
  }

  if (errorCode) {
    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(errorCode.value()));
  }

  response->set_success(success.value());

  return grpc::Status::OK;
}

}  // namespace cmd
}  // namespace hub
