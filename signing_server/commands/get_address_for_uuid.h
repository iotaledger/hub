/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_SIGNING_SERVER_GET_ADDRESS_FOR_UUID_H_
#define HUB_COMMANDS_SIGNING_SERVER_GET_ADDRESS_FOR_UUID_H_

#include <string>

#include "hub/commands/command.h"

namespace signing {
namespace rpc {
class GetAddressForUUIDRequest;
class GetAddressForUUIDReply;
}  // namespace rpc

namespace cmd {

/// Gets information on an address
/// @param[in] hub::rpc::GetAddressForUUIDRequest
/// @param[in] hub::rpc::GetAddressForUUIDReply
class GetAddressForUUID
    : public hub::cmd::Command<signing::rpc::GetAddressForUUIDRequest,
                               signing::rpc::GetAddressForUUIDReply> {
 public:
  using hub::cmd::Command<signing::rpc::GetAddressForUUIDRequest,
                          signing::rpc::GetAddressForUUIDReply>::Command;

  const std::string name() override { return "GetAddressForUUID"; }

  grpc::Status doProcess(
      const signing::rpc::GetAddressForUUIDRequest* request,
      signing::rpc::GetAddressForUUIDReply* response) noexcept override;
};
}  // namespace cmd
}  // namespace signing

#endif  // HUB_COMMANDS_SIGNING_SERVER_GET_ADDRESS_FOR_UUID_H_
