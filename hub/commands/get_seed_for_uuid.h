/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_GET_SEED_FOR_UUID_H_
#define HUB_COMMANDS_GET_SEED_FOR_UUID_H_

#include <string>

#include "common/commands/command.h"

namespace hub {
namespace cmd {

typedef struct GetSeedForUUIDRequest {
  std::string uuid;
} GetSeedForUUIDRequest;

typedef struct GetSeedForUUIDReply {
  std::string seed;
} GetSeedForUUIDReply;

class GetSeedForUUID
    : public common::Command<GetSeedForUUIDRequest, GetSeedForUUIDReply> {
 public:
  using Command<GetSeedForUUIDRequest, GetSeedForUUIDReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new GetSeedForUUID(std::make_shared<common::ClientSession>()));
  }

  static const std::string name() { return "GetSeedForUUID"; }

  common::cmd::Error doProcess(const GetSeedForUUIDRequest* request,
                               GetSeedForUUIDReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_GET_SEED_FOR_UUID_H_
