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

namespace {
SQLPP_ALIAS_PROVIDER(total);
}

namespace iota {
namespace cmd {

grpc::Status GetBalance::doProcess(
    const iota::rpc::GetBalanceRequest* request,
    iota::rpc::GetBalanceReply* response) noexcept {
  db::sql::UserAccountBalance bal;

  auto& connection = db::DBManager::get().connection();
  uint64_t userId;

  // Get userId for identifier
  {
    auto maybeUserId = db::userIdFromIdentifier(connection, request->userid());
    if (!maybeUserId) {
      return grpc::Status(
          grpc::StatusCode::FAILED_PRECONDITION, "",
          errorToString(iota::rpc::ErrorCode::USER_DOES_NOT_EXIST));
    }

    userId = maybeUserId.value();
  }

  // Summarise all amounts for user_account_balance changes
  {
    const auto result = connection(select(sum(bal.amount).as(total))
                                       .from(bal)
                                       .where(bal.userAccount == userId));

    if (result.empty()) {
      return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                          errorToString(iota::rpc::ErrorCode::UNKNOWN));
    }

    response->set_available(result.front().total);
  }

  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace iota
