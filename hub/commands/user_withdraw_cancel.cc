#include "user_withdraw_cancel.h"

#include <cstdint>

#include <sqlpp11/connection.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/stats/session.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "helper.h"

using namespace sqlpp;

namespace hub {
namespace cmd {

grpc::Status UserWithdrawCancel::doProcess(
    const hub::rpc::UserWithdrawCancelRequest* request,
    hub::rpc::UserWithdrawCancelReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();
  transaction_t<hub::db::Connection> transaction(connection, true);

  std::optional<hub::rpc::ErrorCode> errorCode;
  boost::uuids::uuid uuid = boost::uuids::string_generator()(request->uuid());

  try {
    auto result = db::cancelWithdrawal(connection, uuid);

    if (result == 0) {
      errorCode = hub::rpc::ErrorCode::CANCELLATION_NOT_POSSIBLE;
    }

  cleanup:
    if (errorCode) {
      transaction.rollback();
    } else {
      transaction.commit();
    }
  } catch (sqlpp::exception& ex) {
    LOG(ERROR) << session() << " Commit failed: " << ex.what();
    errorCode = hub::rpc::ErrorCode::UNKNOWN;
  }

  if (errorCode) {
    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(errorCode.value()));
  }

  response->set_success(true);

  return grpc::Status::CANCELLED;
}

}  // namespace cmd
}  // namespace hub
