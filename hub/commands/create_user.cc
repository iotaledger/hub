/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/create_user.h"

#include <sqlpp11/connection.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/transaction.h>

#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "common/stats/session.h"
#include "hub/db/db.h"

#include "hub/commands/helper.h"

namespace hub {
namespace cmd {

grpc::Status CreateUser::doProcess(
    const hub::rpc::CreateUserRequest* request,
    hub::rpc::CreateUserReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();

  auto transaction = connection.transaction();

  try {
    connection.createUser(request->userid());
    transaction->commit();
  } catch (const sqlpp::exception& ex) {
    LOG(ERROR) << session() << " Commit failed: " << ex.what();

    try {
      transaction->rollback();
    } catch (const sqlpp::exception& ex) {
      LOG(ERROR) << session() << " Rollback failed: " << ex.what();
    }

    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(hub::rpc::ErrorCode::USER_EXISTS));
  }

  LOG(INFO) << session() << " Created user: " << request->userid();

  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace hub
