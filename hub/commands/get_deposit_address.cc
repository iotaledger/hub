/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/get_deposit_address.h"

#include <sqlpp11/exception.h>
#include <cstdint>

#include "common/crypto/manager.h"
#include "common/stats/session.h"
#include "common/types/types.h"
#include "hub/commands/helper.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

namespace hub {
namespace cmd {

grpc::Status GetDepositAddress::doProcess(
    const hub::rpc::GetDepositAddressRequest* request,
    hub::rpc::GetDepositAddressReply* response) noexcept {
  uint64_t userId;

  auto& connection = db::DBManager::get().connection();

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

  common::crypto::UUID uuid;
  auto maybeAddress =
      common::crypto::CryptoManager::get().provider().getAddressForUUID(uuid);
  if (!maybeAddress.has_value()) {
    LOG(ERROR) << session() << " Failed in getAddressForUUID from provider.";
    return grpc::Status(
        grpc::StatusCode::UNAVAILABLE, "",
        errorToString(hub::rpc::ErrorCode::RPC_CALL_SIGNING_SERVER_FAILED));
  }
  auto address = maybeAddress.value();
  if (request->includechecksum()) {
    response->set_address(address.str() + common::crypto::CryptoManager::get()
                                              .provider()
                                              .calcChecksum(address.str())
                                              .str());
  } else {
    response->set_address(address.str());
  }

  // Add new user address.
  auto transaction = connection.transaction();
  try {
    connection.createUserAddress(address, userId, uuid);
    transaction->commit();
  } catch (sqlpp::exception& ex) {
    LOG(ERROR) << session() << " Commit failed: " << ex.what();

    try {
      transaction->rollback();
    } catch (const sqlpp::exception& ex) {
      LOG(ERROR) << session() << " Rollback failed: " << ex.what();
    }

    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(hub::rpc::ErrorCode::EC_UNKNOWN));
  }

  return grpc::Status::OK;
}
}  // namespace cmd
}  // namespace hub
