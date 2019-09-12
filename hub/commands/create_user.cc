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

#include "hub/commands/factory.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<CreateUser> registrator;

boost::property_tree::ptree CreateUser::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  CreateUserRequest req;
  CreateUserReply rep;
  auto maybeUserId = request.get_optional<std::string>("userId");
  if (maybeUserId) {
    req.userId = maybeUserId.value();
  }
  auto status = doProcess(&req, &rep);

  return tree;
}

common::cmd::Error CreateUser::doProcess(const CreateUserRequest* request,
                                         CreateUserReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();

  auto transaction = connection.transaction();

  try {
    connection.createUser(request->userId);
    transaction->commit();
  } catch (const sqlpp::exception& ex) {
    LOG(ERROR) << session() << " Commit failed: " << ex.what();

    try {
      transaction->rollback();
    } catch (const sqlpp::exception& ex) {
      LOG(ERROR) << session() << " Rollback failed: " << ex.what();
    }

    return common::cmd::USER_EXISTS;
  }

  LOG(INFO) << session() << " Created user: " << request->userId;

  return common::cmd::OK;
}

}  // namespace cmd

}  // namespace hub
