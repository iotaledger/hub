/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/get_user_history.h"

#include <cstdint>

#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/stats/session.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/helper.h"
#include "hub/commands/proto_sql_converter.h"

namespace hub {
namespace cmd {

grpc::Status GetUserHistory::doProcess(
    const hub::rpc::GetUserHistoryRequest* request,
    hub::rpc::GetUserHistoryReply* response) noexcept {
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

  {
    auto balances = connection.getUserAccountBalances(userId);

    for (const auto& b : balances) {
      auto* event = response->add_events();
      event->set_timestamp(
          std::chrono::duration_cast<std::chrono::milliseconds>(
              b.timestamp.time_since_epoch())
              .count());
      event->set_amount(b.amount);
      event->set_type(userAccountBalanceEventTypeFromSql(b.type));
    }
  }

  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace hub
