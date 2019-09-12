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

#include "common/crypto/manager.h"
#include "common/crypto/types.h"
#include "common/stats/session.h"
#include "hub/auth/hmac_provider.h"
#include "hub/auth/manager.h"
#include "hub/commands/factory.h"
#include "hub/commands/helper.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<SignBundle> registrator;

boost::property_tree::ptree SignBundle::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  return tree;
}

common::cmd::Error SignBundle::doProcess(
    const hub::rpc::SignBundleRequest* request,
    hub::rpc::SignBundleReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();
  auto& cryptoProvider = common::crypto::CryptoManager::get().provider();
  auto& authProvider = auth::AuthManager::get().provider();

  try {
    nonstd::optional<common::crypto::Address> address;
    if (request->validatechecksum()) {
      address = std::move(common::crypto::CryptoManager::get()
                              .provider()
                              .verifyAndStripChecksum(request->address()));

      if (!address.has_value()) {
        return common::cmd::INVALID_CHECKSUM;
      }
    } else {
      address = {common::crypto::Address(request->address())};
    }

    common::crypto::Hash bundleHash(request->bundlehash());

    // 1. Check that address was used before
    auto maybeAddressInfo = connection.getAddressInfo(address.value());
    if (!maybeAddressInfo) {
      return common::cmd::UNKNOWN_ADDRESS;
    }

    if (!maybeAddressInfo->usedForSweep) {
      return common::cmd::INVALID_ADDRESS;
    }

    // 2. Verify authentication token
    if (!authProvider.validateToken(
            auth::SignBundleContext(bundleHash, address.value()),
            request->authentication())) {
      return common::cmd::INVALID_AUTHENTICATION;
    }

    // 3. Calculate signature
    auto maybeSig = cryptoProvider.forceGetSignatureForUUID(
        maybeAddressInfo->uuid, bundleHash);
    if (!maybeSig.has_value()) {
      return common::cmd::SIGNATURE_FAILED;
    }
    response->set_signature(maybeSig.value());
  } catch (const std::exception& ex) {
    return common::cmd::UNKNOWN_ERROR;
  }

  return common::cmd::OK;
}

}  // namespace cmd
}  // namespace hub
