/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_SIGNING_SERVER_GET_SECURITY_LEVEL_H_
#define HUB_COMMANDS_SIGNING_SERVER_GET_SECURITY_LEVEL_H_

#include <string>

#include "hub/commands/command.h"

namespace signing {
namespace rpc {
class GetSecurityLevelRequest;
class GetSecurityLevelReply;
}  // namespace rpc

namespace cmd {

/// Gets information on an signature
/// @param[in] hub::rpc::GetSignatureForUUIDRequest
/// @param[in] hub::rpc::GetSignatureForUUIDReply
class GetSecurityLevel
    : public hub::cmd::Command<signing::rpc::GetSecurityLevelRequest,
                               signing::rpc::GetSecurityLevelReply> {
 public:
  using hub::cmd::Command<signing::rpc::GetSecurityLevelRequest,
                          signing::rpc::GetSecurityLevelReply>::Command;

  const std::string name() override { return "GetSignatureForUUID"; }

  grpc::Status doProcess(
      const signing::rpc::GetSecurityLevelRequest* request,
      signing::rpc::GetSecurityLevelReply* response) noexcept override;
};
}  // namespace cmd
}  // namespace signing

#endif  // HUB_COMMANDS_SIGNING_SERVER_GET_SECURITY_LEVEL_H_
