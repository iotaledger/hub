/*
 * Copyright (c) 2018 IOTA Stiftung
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

#include "hub/commands/get_deposit_address.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<GetDepositAddress> registrator;

boost::property_tree::ptree GetDepositAddress::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;

  GetDepositAddressRequest req;
  GetDepositAddressReply rep;
  auto maybeUserId = request.get_optional<std::string>("userId");
  if (!maybeUserId) {
    tree.add("error",
             common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
    return tree;
  }

  req.userId = maybeUserId.value();

  req.includeChecksum = false;
  auto maybeIncludeChecksum =
      request.get_optional<std::string>("includeChecksum");
  if (maybeIncludeChecksum) {
    req.includeChecksum = common::stringToBool(maybeIncludeChecksum.value());
  }
  auto status = doProcess(&req, &rep);

  if (status != common::cmd::OK) {
    tree.add("error", common::cmd::getErrorString(status));
  } else {
    tree.add("address", rep.address);
  }
  return tree;
}

common::cmd::Error GetDepositAddress::doProcess(
    const GetDepositAddressRequest* request,
    GetDepositAddressReply* response) noexcept {
  uint64_t userId;

  auto& connection = db::DBManager::get().connection();

  // Get userId for identifier
  {
    auto maybeUserId = connection.userIdFromIdentifier(request->userId);
    if (!maybeUserId) {
      return common::cmd::USER_DOES_NOT_EXIST;
    }

    userId = maybeUserId.value();
  }

  common::crypto::UUID uuid;
  auto maybeAddress =
      common::crypto::CryptoManager::get().provider().getAddressForUUID(uuid);
  if (!maybeAddress.has_value()) {
    LOG(ERROR) << session() << " Failed in getAddressForUUID from provider.";
    return common::cmd::GET_ADDRESS_FAILED;
  }
  auto address = maybeAddress.value();
  if (request->includeChecksum) {
    response->address = address.str() + common::crypto::CryptoManager::get()
                                            .provider()
                                            .calcChecksum(address.str())
                                            .str();
  } else {
    response->address = address.str();
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

    return common::cmd::UNKNOWN_ERROR;
  }

  return common::cmd::OK;
}
}  // namespace cmd
}  // namespace hub
