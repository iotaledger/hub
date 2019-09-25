/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include <cstdint>

#include "hub/commands/factory.h"
#include "hub/commands/helper.h"
#include "hub/db/helper.h"

#include "hub/commands/get_balance.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<GetBalance> registrator;

boost::property_tree::ptree GetBalance::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  GetBalanceRequest req;
  GetBalanceReply rep;

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
  } else {
    tree.add("available", rep.available);
  }
  return tree;
}

common::cmd::Error GetBalance::doProcess(const GetBalanceRequest* request,
                                         GetBalanceReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();
  uint64_t userId;

  // Get userId for identifier
  {
    auto maybeUserId = connection.userIdFromIdentifier(request->userId);
    if (!maybeUserId) {
      return common::cmd::USER_DOES_NOT_EXIST;
    }

    userId = maybeUserId.value();
  }

  // Summarise all amounts for user_account_balance changes
  auto amount = connection.availableBalanceForUser(userId);
  response->available = amount;

  return common::cmd::OK;
}

}  // namespace cmd

}  // namespace hub
