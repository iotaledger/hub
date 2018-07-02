/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/get_balance.h"

#include <cstdint>

#include <sqlpp11/connection.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

#include "common/stats/session.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/helper.h"

namespace hub {
namespace cmd {

grpc::Status GetBalance::doProcess(
    const hub::rpc::GetBalanceRequest* request,
    hub::rpc::GetBalanceReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();
  uint64_t userId;

  // Get userId for identifier
  {
    auto maybeUserId = connection.userIdFromIdentifier(request->userid());
    if (!maybeUserId) {
      return grpc::Status(
          grpc::StatusCode::FAILED_PRECONDITION, "",
          errorToString(hub::rpc::ErrorCode::USER_DOES_NOT_EXIST));
    }

    userId = maybeUserId.value();
  }

  // Summarise all amounts for user_account_balance changes
  auto amount = connection.availableBalanceForUser(userId);
  response->set_available(amount);

  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace hub
