/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_SIGNING_SERVER_GET_SIGNATURE_FOR_UUID_H_
#define HUB_COMMANDS_SIGNING_SERVER_GET_SIGNATURE_FOR_UUID_H_

#include <string>

#include "hub/commands/command.h"

namespace signing {
namespace rpc {
class GetSignatureForUUIDRequest;
class GetSignatureForUUIDReply;
}  // namespace rpc

namespace cmd {

/// Gets information on an signature
/// @param[in] hub::rpc::GetSignatureForUUIDRequest
/// @param[in] hub::rpc::GetSignatureForUUIDReply
class GetSignatureForUUID
    : public hub::cmd::Command<signing::rpc::GetSignatureForUUIDRequest,
                               signing::rpc::GetSignatureForUUIDReply> {
 public:
  using hub::cmd::Command<signing::rpc::GetSignatureForUUIDRequest,
                          signing::rpc::GetSignatureForUUIDReply>::Command;

  const std::string name() override { return "GetSignatureForUUID"; }

  grpc::Status doProcess(
      const signing::rpc::GetSignatureForUUIDRequest* request,
      signing::rpc::GetSignatureForUUIDReply* response) noexcept override;
};
}  // namespace cmd
}  // namespace signing

#endif  // HUB_COMMANDS_SIGNING_SERVER_GET_SIGNATURE_FOR_UUID_H_
