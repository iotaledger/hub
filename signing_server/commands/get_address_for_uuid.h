/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef SIGNING_SERVER_COMMANDS_GET_ADDRESS_FOR_UUID_H_
#define SIGNING_SERVER_COMMANDS_GET_ADDRESS_FOR_UUID_H_

#include <string>

#include "common/command.h"

namespace signing {
namespace rpc {
class GetAddressForUUIDRequest;
class GetAddressForUUIDReply;
}  // namespace rpc

namespace cmd {

/// Gets information on an address
/// @param[in] common::rpc::GetAddressForUUIDRequest
/// @param[in] common::rpc::GetAddressForUUIDReply
class GetAddressForUUID
    : public common::Command<signing::rpc::GetAddressForUUIDRequest,
                             signing::rpc::GetAddressForUUIDReply> {
 public:
  using common::Command<signing::rpc::GetAddressForUUIDRequest,
                        signing::rpc::GetAddressForUUIDReply>::Command;

  const std::string name() override { return "GetAddressForUUID"; }

  grpc::Status doProcess(
      const signing::rpc::GetAddressForUUIDRequest* request,
      signing::rpc::GetAddressForUUIDReply* response) noexcept override;
};
}  // namespace cmd
}  // namespace signing

#endif  // SIGNING_SERVER_COMMANDS_GET_ADDRESS_FOR_UUID_H_
