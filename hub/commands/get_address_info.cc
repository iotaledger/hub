/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include <cstdint>

#include "common/crypto/manager.h"
#include "common/crypto/types.h"
#include "hub/commands/factory.h"
#include "hub/commands/helper.h"
#include "hub/db/helper.h"

#include "hub/commands/get_address_info.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<GetAddressInfo> registrator;

boost::property_tree::ptree GetAddressInfo::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  GetAddressInfoRequest req;
  GetAddressInfoReply rep;

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
    tree.add("userId", rep.userId);
  }

  return tree;
}

common::cmd::Error GetAddressInfo::doProcess(
    const GetAddressInfoRequest* request,
    GetAddressInfoReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();

  try {
    common::crypto::Address address(request->address);

    auto addressInfo = connection.getAddressInfo(address);

    if (addressInfo) {
      response->userId = std::move(addressInfo->userId);
      return common::cmd::OK;
    }
  } catch (const std::exception& ex) {
    return common::cmd::UNKNOWN_ERROR;
  }

  return common::cmd::UNKNOWN_ADDRESS;
}

}  // namespace cmd
}  // namespace hub
