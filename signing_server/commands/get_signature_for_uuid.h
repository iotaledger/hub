/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef SIGNING_SERVER_COMMANDS_GET_SIGNATURE_FOR_UUID_H_
#define SIGNING_SERVER_COMMANDS_GET_SIGNATURE_FOR_UUID_H_

#include <string>

#include "common/commands/command.h"

namespace signing {
namespace rpc {
class GetSignatureForUUIDRequest;
class GetSignatureForUUIDReply;
}  // namespace rpc

namespace cmd {

/// Gets information on an signature
/// @param[in] common::rpc::GetSignatureForUUIDRequest
/// @param[in] common::rpc::GetSignatureForUUIDReply
class GetSignatureForUUID
    : public common::Command<signing::rpc::GetSignatureForUUIDRequest,
                             signing::rpc::GetSignatureForUUIDReply> {
 public:
  using common::Command<signing::rpc::GetSignatureForUUIDRequest,
                        signing::rpc::GetSignatureForUUIDReply>::Command;

  static const std::string name() { return "GetSignatureForUUID"; }

    common::cmd::Error doProcess(
      const signing::rpc::GetSignatureForUUIDRequest* request,
      signing::rpc::GetSignatureForUUIDReply* response) noexcept override;

    boost::property_tree::ptree doProcess(
            const boost::property_tree::ptree& request) noexcept override {return boost::property_tree::ptree();};
};
}  // namespace cmd
}  // namespace signing

#endif  // SIGNING_SERVER_COMMANDS_GET_SIGNATURE_FOR_UUID_H_
