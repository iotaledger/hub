/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include <cstdint>

#include "common/crypto/manager.h"
#include "hub/commands/factory.h"
#include "hub/commands/helper.h"

#include "hub/commands/get_seed_for_uuid.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<GetSeedForUUID> registrator;

boost::property_tree::ptree GetSeedForUUID::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  GetSeedForUUIDRequest req;
  GetSeedForUUIDReply rep;

  auto maybeUUID = request.get_optional<std::string>("uuid");
  if (!maybeUUID) {
    tree.add("error",
             common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
    return tree;
  }

  req.uuid = maybeUUID.value();

  auto status = doProcess(&req, &rep);

  if (status != common::cmd::OK) {
    tree.add("error", common::cmd::getErrorString(status));
  } else {
    tree.add("seed", rep.seed);
  }
  return tree;
}

common::cmd::Error GetSeedForUUID::doProcess(
    const GetSeedForUUIDRequest* request,
    GetSeedForUUIDReply* response) noexcept {
  // Get seed by the uuid
  response->seed =
      common::crypto::CryptoManager::get().provider().getSeedFromUUID(
          common::crypto::UUID(request->uuid));

  return common::cmd::OK;
}
}  // namespace cmd
}  // namespace hub
