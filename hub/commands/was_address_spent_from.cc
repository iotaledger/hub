/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/was_address_spent_from.h"

#include <cstdint>
#include <exception>

#include "common/stats/session.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "common/crypto/manager.h"
#include "common/crypto/types.h"
#include "hub/commands/helper.h"

namespace hub {
namespace cmd {

grpc::Status WasAddressSpentFrom::doProcess(
    const hub::rpc::WasAddressSpentFromRequest* request,
    hub::rpc::WasAddressSpentFromReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();

  nonstd::optional<hub::rpc::ErrorCode> errorCode;

  nonstd::optional<common::crypto::Address> address;

  try {
    if (request->validatechecksum()) {
      address = std::move(common::crypto::CryptoManager::get()
                              .provider()
                              .verifyAndStripChecksum(request->address()));

      if (!address.has_value()) {
        return grpc::Status(
            grpc::StatusCode::FAILED_PRECONDITION, "",
            errorToString(hub::rpc::ErrorCode::CHECKSUM_INVALID));
      }
    } else {
      address = {common::crypto::Address(request->address())};
    }
  }

  catch (const std::exception& ex) {
    LOG(ERROR) << session() << " Withdrawal to invalid address: " << ex.what();

    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(hub::rpc::ErrorCode::EC_UNKNOWN));
  }

  if (!isAddressValid(address->str_view())) {
    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(hub::rpc::ErrorCode::INELIGIBLE_ADDRESS));
  }

  response->set_wasaddressspentfrom(false);
  try {
    // Verify address wasn't spent before
    if (_api) {
      auto res = _api->wereAddressesSpentFrom({address.value().str()});
      if (!res.has_value() || res.value().states.empty()) {
        errorCode = hub::rpc::ErrorCode::IRI_CLIENT_UNAVAILABLE;
      } else if (res.value().states.front()) {
        errorCode = hub::rpc::ErrorCode::ADDRESS_WAS_ALREADY_SPENT;
        response->set_wasaddressspentfrom(true);
      }
    }

  } catch (const std::exception& ex) {
    LOG(ERROR) << session() << " Commit failed: " << ex.what();

    errorCode = hub::rpc::ErrorCode::EC_UNKNOWN;
  }

done:
  if (errorCode) {
    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(errorCode.value()));
  }

  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace hub
