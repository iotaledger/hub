/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef SIGNING_SERVER_COMMANDS_GET_SECURITY_LEVEL_H_
#define SIGNING_SERVER_COMMANDS_GET_SECURITY_LEVEL_H_

#include <string>

#include "common/commands/command.h"

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
    : public common::Command<signing::rpc::GetSecurityLevelRequest,
                             signing::rpc::GetSecurityLevelReply> {
 public:
  using common::Command<signing::rpc::GetSecurityLevelRequest,
                        signing::rpc::GetSecurityLevelReply>::Command;

  static const std::string name() { return "GetSecurityLevel"; }

    common::cmd::Error doProcess(
      const signing::rpc::GetSecurityLevelRequest* request,
      signing::rpc::GetSecurityLevelReply* response) noexcept override;

    boost::property_tree::ptree doProcess(
            const boost::property_tree::ptree& request) noexcept override {return boost::property_tree::ptree();};
};
}  // namespace cmd
}  // namespace signing

#endif  // SIGNING_SERVER_COMMANDS_GET_SECURITY_LEVEL_H_
