/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include <cstdint>

#include "common/converter.h"
#include "common/crypto/manager.h"
#include "common/crypto/types.h"
#include "hub/commands/factory.h"
#include "hub/commands/helper.h"
#include "hub/db/helper.h"

#include "hub/commands/get_seed_for_address.h"

namespace hub {
namespace cmd {

DEFINE_bool(GetSeedForAddress_enabled, false,
            "Whether the RecoverFunds API call should be available");

static CommandFactoryRegistrator<GetSeedForAddress> registrator;

boost::property_tree::ptree GetSeedForAddress::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  GetSeedForAddressRequest req;
  GetSeedForAddressReply rep;

  if (!FLAGS_GetSeedForAddress_enabled) {
    LOG(ERROR) << session() << ": GetSeedForAddress is disabled";
    tree.add("error", common::cmd::getErrorString(common::cmd::CANCELLED));
    return tree;
  }

  auto maybeUserId = request.get_optional<std::string>("userId");
  if (!maybeUserId) {
    tree.add("error",
             common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
    return tree;
  }

  req.userId = maybeUserId.value();

  auto maybeAddress = request.get_optional<std::string>("address");
  if (!maybeAddress) {
    tree.add("error",
             common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
    return tree;
  }

  req.address = maybeAddress.value();

  auto status = doProcess(&req, &rep);

  if (status != common::cmd::OK) {
    tree.add("error", common::cmd::getErrorString(status));
  } else {
    tree.add("seed", rep.seed);
  }
  return tree;
}

common::cmd::Error GetSeedForAddress::doProcess(
    const GetSeedForAddressRequest* request,
    GetSeedForAddressReply* response) noexcept {
  if (!FLAGS_GetSeedForAddress_enabled) {
    LOG(ERROR) << session() << ": GetSeedForAddress is disabled";
    return common::cmd::CANCELLED;
  }

  auto& connection = db::DBManager::get().connection();

  // Get userId for identifier
  {
    auto maybeUserId = connection.userIdFromIdentifier(request->userId);
    if (!maybeUserId) {
      return common::cmd::USER_DOES_NOT_EXIST;
    }
  }

  nonstd::optional<common::crypto::Address> maybeTrinaryAddress =
      nonstd::nullopt;

  try {
    maybeTrinaryAddress = {common::crypto::Address(request->address)};
  } catch (const std::exception& ex) {
    LOG(ERROR) << session() << "Invalid address: " << ex.what();
    return common::cmd::INVALID_ADDRESS;
  }

  auto maybeAddressInfo =
      connection.getAddressInfo(maybeTrinaryAddress.value());
  if (!maybeAddressInfo) {
    return common::cmd::UNKNOWN_ADDRESS;
  }

  if (maybeAddressInfo->userId.compare(request->userId) != 0) {
    return common::cmd::WRONG_USER_ADDRESS;
  }

  response->seed =
      common::crypto::CryptoManager::get().provider().getSeedFromUUID(
          maybeAddressInfo.value().uuid);

  return common::cmd::OK;
}
}  // namespace cmd
}  // namespace hub
