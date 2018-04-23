#include "get_balance.h"

#include <cstdint>

#include <sqlpp11/connection.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/stats/session.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "helper.h"

using namespace sqlpp;

namespace hub {
namespace cmd {

grpc::Status GetBalance::doProcess(
    const hub::rpc::GetBalanceRequest* request,
    hub::rpc::GetBalanceReply* response) noexcept {

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

  // Summarise all amounts for user_account_balance changes
  {
    auto maybeAmount = db::availableBalanceForUser(connection, userId);

    if (!maybeAmount) {
      return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                          errorToString(hub::rpc::ErrorCode::UNKNOWN));
    }

    response->set_available(maybeAmount.value());
  }

  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace hub
