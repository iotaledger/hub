// Copyright 2018 IOTA Foundation

#include "hub/commands/get_user_history.h"

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
#include "hub/commands/proto_sql_convertor.h"

namespace hub {
namespace cmd {

grpc::Status GetUserHistory::doProcess(
    const hub::rpc::GetUserHistoryRequest* request,
    hub::rpc::GetUserHistoryReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();
  uint64_t userId;

  // Get userId for identifier
  {
    auto maybeUserId = db::userIdFromIdentifier(connection, request->userid());
    if (!maybeUserId) {
      return grpc::Status(
          grpc::StatusCode::FAILED_PRECONDITION, "",
          errorToString(hub::rpc::ErrorCode::USER_DOES_NOT_EXIST));
    }

    userId = maybeUserId.value();
  }

  {
    auto balances = db::getUserAccountBalances(connection, userId);

    for (const auto& b : balances) {
      auto* event = response->add_events();
      event->set_timestamp(
          std::chrono::duration_cast<std::chrono::milliseconds>(
              b._timestamp.time_since_epoch())
              .count());
      event->set_amount(b._amount);
      event->set_type(userBalanceEventTypeFromSql(b._type));
    }
  }

  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace hub
