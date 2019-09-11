/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_GET_ADDRESS_INFO_H_
#define HUB_COMMANDS_GET_ADDRESS_INFO_H_

#include <string>

#include "common/command.h"

namespace hub {
namespace rpc {
class GetAddressInfoRequest;
class GetAddressInfoReply;
}  // namespace rpc

namespace cmd {

/// Gets information on an address
/// @param[in] hub::rpc::GetAddressInfoRequest
/// @param[in] hub::rpc::GetAddressInfoReply
class GetAddressInfo : public common::Command<hub::rpc::GetAddressInfoRequest,
                                              hub::rpc::GetAddressInfoReply> {
 public:
  using Command<hub::rpc::GetAddressInfoRequest,
                hub::rpc::GetAddressInfoReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(new GetAddressInfo());
  }

  static const std::string name() { return "GetAddressInfo"; }

  grpc::Status doProcess(
      const hub::rpc::GetAddressInfoRequest* request,
      hub::rpc::GetAddressInfoReply* response) noexcept override;

  std::string doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_GET_ADDRESS_INFO_H_
