#include "create_user.h"

#include <sqlpp11/connection.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/transaction.h>

#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/db/db.h"
#include "hub/stats/session.h"

#include "helper.h"
using namespace sqlpp;

namespace hub {
namespace cmd {

grpc::Status CreateUser::doProcess(
    const hub::rpc::CreateUserRequest* request,
    hub::rpc::CreateUserReply* response) noexcept {
  db::sql::UserAccount userAccount;

  auto& connection = db::DBManager::get().connection();

  transaction_t<hub::db::Connection> transaction(connection, true);

  try {
    connection(insert_into(userAccount)
                   .set(userAccount.identifier = request->userid()));
    transaction.commit();
  } catch (sqlpp::exception& ex) {
    LOG(ERROR) << session() << " Commit failed: " << ex.what();
    transaction.rollback();

    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(hub::rpc::ErrorCode::USER_EXISTS));
  }

  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace hub
