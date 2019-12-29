/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_GET_SEED_FOR_ADDRESS_H_
#define HUB_COMMANDS_GET_SEED_FOR_ADDRESS_H_

#include <string>

#include "common/commands/command.h"

namespace hub {
namespace cmd {

typedef struct GetSeedForAddressRequest {
  std::string userId;
  std::string address;
} GetSeedForAddressRequest;

typedef struct GetSeedForAddressReply {
  std::string seed;
} GetSeedForAddressReply;

class GetSeedForAddress
    : public common::Command<GetSeedForAddressRequest, GetSeedForAddressReply> {
 public:
  using Command<GetSeedForAddressRequest, GetSeedForAddressReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new GetSeedForAddress(std::make_shared<common::ClientSession>()));
  }

  static const std::string name() { return "GetSeedForAddress"; }

  common::cmd::Error doProcess(
      const GetSeedForAddressRequest* request,
      GetSeedForAddressReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_GET_SEED_FOR_ADDRESS_H_
