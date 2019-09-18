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
#include "schema/schema.h"

#include "common/crypto/manager.h"
#include "common/crypto/types.h"
#include "hub/commands/factory.h"
#include "hub/commands/helper.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<WasAddressSpentFrom> registrator;

boost::property_tree::ptree WasAddressSpentFrom::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  WasAddressSpentFromRequest req;
  WasAddressSpentFromReply rep;
  auto maybeAddress = request.get_optional<std::string>("address");
  if (maybeAddress) {
    req.address = maybeAddress.value();
  }

  auto maybeValidateChecksum =
      request.get_optional<std::string>("validateChecksum");
  if (maybeValidateChecksum) {
    req.validateChecksum = (maybeValidateChecksum.value().compare("true") == 0);
  }

  auto status = doProcess(&req, &rep);

  if (status != common::cmd::OK) {
    tree.add("error", common::cmd::errorToStringMap.at(status));
  } else {
    tree.add("wasAddressSpentFrom", rep.wasAddressSpentFrom ? "true" : "false");
  }
  return tree;
}

common::cmd::Error WasAddressSpentFrom::doProcess(
    const WasAddressSpentFromRequest* request,
    WasAddressSpentFromReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();

  nonstd::optional<common::cmd::Error> errorCode;

  nonstd::optional<common::crypto::Address> address;

  try {
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
  }

  catch (const std::exception& ex) {
    LOG(ERROR) << session() << " Withdrawal to invalid address: " << ex.what();

    return common::cmd::UNKNOWN_ERROR;
  }

  if (!isAddressValid(address->str_view())) {
    return common::cmd::INVALID_ADDRESS;
  }

  response->wasAddressSpentFrom = false;
  try {
    // Verify address wasn't spent before
    if (_api) {
      auto res = _api->wereAddressesSpentFrom({address.value().str()});
      if (!res.has_value() || res.value().states.empty()) {
        errorCode = common::cmd::IOTA_NODE_UNAVAILABLE;
      } else if (res.value().states.front()) {
        errorCode = common::cmd::ADDRESS_WAS_SPENT;
        response->wasAddressSpentFrom = true;
      }
    }

  } catch (const std::exception& ex) {
    LOG(ERROR) << session() << " Commit failed: " << ex.what();

    errorCode = common::cmd::UNKNOWN_ERROR;
  }

done:
  if (errorCode) {
    return errorCode.value();
  }

  return common::cmd::OK;
}

}  // namespace cmd

}  // namespace hub
