/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/get_address_info.h"

#include <cstdint>
#include <utility>

#include <sqlpp11/exception.h>

#include "common/crypto/manager.h"
#include "common/crypto/types.h"
#include "common/stats/session.h"
#include "hub/commands/factory.h"
#include "hub/commands/helper.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<GetAddressInfo> registrator;

boost::property_tree::ptree GetAddressInfo::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  return tree;
}

grpc::Status GetAddressInfo::doProcess(
    const hub::rpc::GetAddressInfoRequest* request,
    hub::rpc::GetAddressInfoReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();

  try {
    common::crypto::Address address(request->address());

    auto addressInfo = connection.getAddressInfo(address);

    if (addressInfo) {
      response->set_userid(std::move(addressInfo->userId));
      return grpc::Status::OK;
    }
  } catch (const std::exception& ex) {
    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(hub::rpc::ErrorCode::EC_UNKNOWN));
  }

  return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                      errorToString(hub::rpc::ErrorCode::UNKNOWN_ADDRESS));
}

}  // namespace cmd
}  // namespace hub
