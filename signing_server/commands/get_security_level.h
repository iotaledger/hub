/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_SIGNING_SERVER_GET_SECURITY_LEVEL_H_
#define HUB_COMMANDS_SIGNING_SERVER_GET_SECURITY_LEVEL_H_

#include <string>

#include "common/command.h"

namespace signing {
namespace rpc {
class GetSecurityLevelRequest;
class GetSecurityLevelReply;
}  // namespace rpc

namespace cmd {

/// Gets information on an signature
/// @param[in] common::rpc::GetSignatureForUUIDRequest
/// @param[in] common::rpc::GetSignatureForUUIDReply
class GetSecurityLevel
    : public common::cmd::Command<signing::rpc::GetSecurityLevelRequest,
                                  signing::rpc::GetSecurityLevelReply> {
 public:
  using common::cmd::Command<signing::rpc::GetSecurityLevelRequest,
                             signing::rpc::GetSecurityLevelReply>::Command;

  const std::string name() override { return "GetSignatureForUUID"; }

  grpc::Status doProcess(
      const signing::rpc::GetSecurityLevelRequest* request,
      signing::rpc::GetSecurityLevelReply* response) noexcept override;
};
}  // namespace cmd
}  // namespace signing

#endif  // HUB_COMMANDS_SIGNING_SERVER_GET_SECURITY_LEVEL_H_
