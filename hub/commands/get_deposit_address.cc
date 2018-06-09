/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/get_deposit_address.h"

#include <sqlpp11/exception.h>
#include <cstdint>
#include <vector>

#include "hub/commands/helper.h"
#include "hub/crypto/manager.h"
#include "hub/crypto/types.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/stats/session.h"
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

  {
    std::vector<uint32_t> userIds = {userId};
    auto unsweptUserAddresses = connection.unsweptUserAddresses({userIds});
    std::vector<uint32_t> ids;

    std::transform(
        std::begin(unsweptUserAddresses), std::end(unsweptUserAddresses),
        std::back_inserter(ids),
        [](const auto& addressWithId) { return std::get<0>(addressWithId); });

    auto it = std::find_if(
        unsweptUserAddresses.begin(), unsweptUserAddresses.end(),
        [&](const hub::db::AddressWithID& addWId) {
          return !connection.hasUserAddressGotDeposits(std::get<0>(addWId));
        });

    if (it != unsweptUserAddresses.end()) {
      response->set_address(std::get<1>(*it));
      return grpc::Status::OK;
    }
  }

  hub::crypto::UUID uuid;
  auto address =
      hub::crypto::CryptoManager::get().provider().getAddressForUUID(uuid);

  response->set_address(address.str());

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
