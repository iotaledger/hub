/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing Secretrmation
 */

#include <cstdint>

#include "common/converter.h"
#include "common/crypto/manager.h"
#include "common/crypto/types.h"
#include "hub/auth/hmac_provider.h"
#include "hub/auth/manager.h"
#include "hub/commands/factory.h"
#include "hub/commands/helper.h"
#include "hub/db/helper.h"

#include "hub/commands/sign_bundle.h"

namespace hub {
namespace cmd {

// Commands
DEFINE_bool(SignBundle_enabled, false,
            "Whether the SignBundle API call should be available");

static CommandFactoryRegistrator<SignBundle> registrator;

boost::property_tree::ptree SignBundle::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;

  if (!FLAGS_SignBundle_enabled) {
    LOG(ERROR) << session() << ": Recover funds is disabled";
    tree.add("error", common::cmd::getErrorString(common::cmd::CANCELLED));
  }

  SignBundleRequest req;
  SignBundleReply rep;
  auto maybeAddress = request.get_optional<std::string>("address");
  if (!maybeAddress) {
    tree.add("error",
             common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
    return tree;
  }

  req.address = maybeAddress.value();
  req.validateChecksum = false;

  auto maybeValidateChecksum =
      request.get_optional<std::string>("validateChecksum");
  if (maybeValidateChecksum) {
    req.validateChecksum = common::stringToBool(maybeValidateChecksum.value());
  }

  auto maybeBundleHash = request.get_optional<std::string>("bundleHash");
  if (maybeBundleHash) {
    tree.add("error",
             common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
    return tree;
  }

  req.bundleHash = maybeBundleHash.value();

  auto maybeAuthenticationToken =
      request.get_optional<std::string>("authenticationToken");
  if (maybeAuthenticationToken) {
    req.authenticationToken = maybeAuthenticationToken.value();
  }
  auto status = doProcess(&req, &rep);

  if (status != common::cmd::OK) {
    tree.add("error", common::cmd::getErrorString(status));
  } else {
    tree.add("signature", rep.signature);
  }
  return tree;
}

common::cmd::Error SignBundle::doProcess(const SignBundleRequest* request,
                                         SignBundleReply* response) noexcept {
  if (!FLAGS_SignBundle_enabled) {
    LOG(ERROR) << session() << ": Recover funds is disabled";
    return common::cmd::CANCELLED;
  }

  auto& connection = db::DBManager::get().connection();
  auto& cryptoProvider = common::crypto::CryptoManager::get().provider();
  auto& authProvider = auth::AuthManager::get().provider();

  try {
    nonstd::optional<common::crypto::Address> address;
    if (request->validateChecksum) {
      address = std::move(common::crypto::CryptoManager::get()
                              .provider()
                              .verifyAndStripChecksum(request->address));

      if (!address.has_value()) {
        return common::cmd::INVALID_CHECKSUM;
      }
    } else {
      address = {common::crypto::Address(request->address)};
    }

    common::crypto::Hash bundleHash(request->bundleHash);

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
            request->authenticationToken)) {
      return common::cmd::INVALID_AUTHENTICATION;
    }

    // 3. Calculate signature
    auto maybeSig = cryptoProvider.forceGetSignatureForUUID(
        maybeAddressInfo->uuid, bundleHash);
    if (!maybeSig.has_value()) {
      return common::cmd::SIGNATURE_FAILED;
    }
    response->signature = maybeSig.value();
  } catch (const std::exception& ex) {
    return common::cmd::UNKNOWN_ERROR;
  }

  return common::cmd::OK;
}

}  // namespace cmd
}  // namespace hub
