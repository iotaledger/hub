/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_GET_DEPOSIT_ADDRESS_H_
#define HUB_COMMANDS_GET_DEPOSIT_ADDRESS_H_

#include <string>

#include "common/commands/command.h"

namespace hub {
namespace cmd {

typedef struct GetDepositAddressRequest {
  GetDepositAddressRequest() : includeChecksum(false) {}
  std::string userId;
  bool includeChecksum;
} GetDepositAddressRequest;

typedef struct GetDepositAddressReply {
  std::string address;
} GetDepositAddressReply;

class GetDepositAddress
    : public common::Command<GetDepositAddressRequest, GetDepositAddressReply> {
 public:
  using Command<GetDepositAddressRequest, GetDepositAddressReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new GetDepositAddress(std::make_shared<common::ClientSession>()));
  }

  static const std::string name() { return "GetDepositAddress"; }

  common::cmd::Error doProcess(
      const GetDepositAddressRequest* request,
      GetDepositAddressReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_GET_DEPOSIT_ADDRESS_H_
