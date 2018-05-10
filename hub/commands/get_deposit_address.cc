// Copyright 2018 IOTA Foundation

#include "hub/commands/get_deposit_address.h"

#include <cstdint>

#include <sqlpp11/connection.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/transaction.h>

#include "hub/commands/helper.h"
#include "hub/crypto/manager.h"
#include "hub/crypto/types.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/stats/session.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

namespace {
SQLPP_ALIAS_PROVIDER(total);
}

namespace hub {
namespace cmd {

grpc::Status GetDepositAddress::doProcess(
    const hub::rpc::GetDepositAddressRequest* request,
    hub::rpc::GetDepositAddressReply* response) noexcept {
  db::sql::UserAddress userAddress;

  uint64_t userId;

  auto& connection = db::DBManager::get().connection();

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

  auto uuid = hub::crypto::UUID::generate();
  auto address =
      hub::crypto::CryptoManager::get().provider().getAddressForUUID(uuid);

  response->set_address(address.toString());

  // Add new user address.
  sqlpp::transaction_t<hub::db::Connection> transaction(connection, true);
  try {
    connection(insert_into(userAddress)
                   .set(userAddress.address = address.toString(),
                        userAddress.userId = userId,
                        userAddress.seedUuid = uuid.toString()));
    transaction.commit();
  } catch (sqlpp::exception& ex) {
    LOG(ERROR) << session() << " Commit failed: " << ex.what();
    transaction.rollback();

    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(hub::rpc::ErrorCode::EC_UNKNOWN));
  }

  return grpc::Status::OK;
}  // namespace cmd

}  // namespace cmd

}  // namespace hub
