/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/factory.h"
#include "hub/commands/helper.h"
#include "hub/db/helper.h"

#include "hub/commands/create_user.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<CreateUser> registrator;

boost::property_tree::ptree CreateUser::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  CreateUserRequest req;
  CreateUserReply rep;
  auto maybeUserId = request.get_optional<std::string>("userId");
  if (!maybeUserId) {
    tree.add("error",
             common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
    return tree;
  }
  req.userId = maybeUserId.value();
  auto status = doProcess(&req, &rep);

  if (status != common::cmd::OK) {
    tree.add("error", common::cmd::getErrorString(status));
  }

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
