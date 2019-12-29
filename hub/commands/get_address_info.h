/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_GET_ADDRESS_INFO_H_
#define HUB_COMMANDS_GET_ADDRESS_INFO_H_

#include <string>

#include "common/commands/command.h"

namespace hub {
namespace cmd {

/// Gets information on an address
/// @param[in] GetAddressInfoRequest
/// @param[in] GetAddressInfoReply

typedef struct GetAddressInfoRequest {
  std::string address;
} GetAddressInfoRequest;

typedef struct GetAddressInfoReply {
  std::string userId;
} GetAddressInfoReply;

class GetAddressInfo
    : public common::Command<GetAddressInfoRequest, GetAddressInfoReply> {
 public:
  using Command<GetAddressInfoRequest, GetAddressInfoReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new GetAddressInfo(std::make_shared<common::ClientSession>()));
  }

  static const std::string name() { return "GetAddressInfo"; }

  common::cmd::Error doProcess(const GetAddressInfoRequest *request,
                               GetAddressInfoReply *response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree &request) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_GET_ADDRESS_INFO_H_
