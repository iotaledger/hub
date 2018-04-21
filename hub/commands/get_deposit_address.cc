#include "get_deposit_address.h"

#include <cstdint>

#include <sqlpp11/connection.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/transaction.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "hub/crypto/manager.h"
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

grpc::Status GetDepositAddress::doProcess(
    const iota::rpc::GetDepositAddressRequest* request,
    iota::rpc::GetDepositAddressReply* response) noexcept {
  db::sql::UserAddress userAddress;

  uint64_t userId;

  auto& connection = db::DBManager::get().connection();

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

  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  auto address =
      iota::crypto::CryptoManager::get().provider().getAddressForUUID(uuid);

  response->set_address(address);

  // Add new user address.
  transaction_t<iota::db::Connection> transaction(connection, true);
  try {
    connection(insert_into(userAddress)
                   .set(userAddress.address = address,
                        userAddress.userAccount = userId,
                        userAddress.seedUuid = (char*) uuid.data));
    transaction.commit();
  } catch (sqlpp::exception& ex) {
    LOG(ERROR) << session() << " Commit failed: " << ex.what();
    transaction.rollback();

    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(iota::rpc::ErrorCode::UNKNOWN));
  }

  return grpc::Status::OK;
}  // namespace cmd

}  // namespace cmd

}  // namespace iota
