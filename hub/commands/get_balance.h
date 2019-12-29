/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_GET_BALANCE_H_
#define HUB_COMMANDS_GET_BALANCE_H_

#include <string>

#include "common/commands/command.h"

namespace hub {

namespace cmd {

typedef struct GetBalanceRequest {
  std::string userId;
} GetBalanceRequest;

typedef struct GetBalanceReply {
  uint64_t available;
} GetBalanceReply;

/// Gets the current balance for a user with a specific id.
/// @param[in] GetBalanceRequest
/// @param[in] GetBalanceReply
class GetBalance : public common::Command<GetBalanceRequest, GetBalanceReply> {
 public:
  using Command<GetBalanceRequest, GetBalanceReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new GetBalance(std::make_shared<common::ClientSession>()));
  }

  static const std::string name() { return "GetBalance"; }

  common::cmd::Error doProcess(const GetBalanceRequest* request,
                               GetBalanceReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_GET_BALANCE_H_
