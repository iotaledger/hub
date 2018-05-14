// Copyright 2018 IOTA Foundation

#include "hub/commands/user_withdraw_cancel.h"

#include <cstdint>

#include <sqlpp11/connection.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/stats/session.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/helper.h"

namespace hub {
namespace cmd {

grpc::Status UserWithdrawCancel::doProcess(
    const hub::rpc::UserWithdrawCancelRequest* request,
    hub::rpc::UserWithdrawCancelReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();
  auto transaction = connection.transaction();

  std::optional<hub::rpc::ErrorCode> errorCode;
  std::optional<bool> success;

  boost::uuids::uuid uuid = boost::uuids::string_generator()(request->uuid());

  try {
    auto result = connection.cancelWithdrawal(boost::uuids::to_string(uuid));

    success = result != 0;

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

  response->set_success(success.value());

  return grpc::Status::OK;
}

}  // namespace cmd
}  // namespace hub
