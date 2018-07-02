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
class GetAddressInfo
    : public common::cmd::Command<hub::rpc::GetAddressInfoRequest,
                                  hub::rpc::GetAddressInfoReply> {
 public:
  using Command<hub::rpc::GetAddressInfoRequest,
                hub::rpc::GetAddressInfoReply>::Command;

  const std::string name() override { return "GetAddressInfo"; }

  grpc::Status doProcess(
      const hub::rpc::GetAddressInfoRequest* request,
      hub::rpc::GetAddressInfoReply* response) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_GET_ADDRESS_INFO_H_
