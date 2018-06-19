/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing Secretrmation
 */

#include "hub/commands/sign_bundle.h"

#include <cstdint>
#include <utility>

#include <sqlpp11/exception.h>

#include "hub/auth/hmac_provider.h"
#include "hub/auth/manager.h"
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

grpc::Status SignBundle::doProcess(
    const hub::rpc::SignBundleRequest* request,
    hub::rpc::SignBundleReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();
  auto& cryptoProvider = crypto::CryptoManager::get().provider();
  auto& authProvider = auth::AuthManager::get().provider();

  try {
    hub::crypto::Address address(request->address());
    hub::crypto::Hash bundleHash(request->bundlehash());

    // 1. Check that address was used before
    auto maybeAddressInfo = connection.getAddressInfo(address);
    if (!maybeAddressInfo) {
      return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                          errorToString(hub::rpc::ErrorCode::UNKNOWN_ADDRESS));
    }

    if (!maybeAddressInfo->usedForSweep) {
      return grpc::Status(
          grpc::StatusCode::FAILED_PRECONDITION, "",
          errorToString(hub::rpc::ErrorCode::INELIGIBLE_ADDRESS));
    }

    // 2. Verify authentication token
    if (!authProvider.validateToken(
            auth::SignBundleContext(bundleHash, address),
            request->authentication())) {
      return grpc::Status(
          grpc::StatusCode::FAILED_PRECONDITION, "",
          errorToString(hub::rpc::ErrorCode::INVALID_AUTHENTICATION));
    }

    // 3. Calculate signature
    response->set_signature(cryptoProvider.forceGetSignatureForUUID(
        maybeAddressInfo->uuid, bundleHash));
  } catch (const std::exception& ex) {
    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(hub::rpc::ErrorCode::EC_UNKNOWN));
  }

  return grpc::Status::OK;
}

}  // namespace cmd
}  // namespace hub
