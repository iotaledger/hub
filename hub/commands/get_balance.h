/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_GET_BALANCE_H_
#define HUB_COMMANDS_GET_BALANCE_H_

#include <string>

#include "common/commands/command.h"

namespace hub {
namespace rpc {
class GetBalanceRequest;
class GetBalanceReply;
}  // namespace rpc

namespace cmd {

/// Gets the current balance for a user with a specific id.
/// @param[in] hub::rpc::GetBalanceRequest
/// @param[in] hub::rpc::GetBalanceReply
class GetBalance : public common::Command<hub::rpc::GetBalanceRequest,
                                          hub::rpc::GetBalanceReply> {
 public:
  using Command<hub::rpc::GetBalanceRequest,
                hub::rpc::GetBalanceReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(new GetBalance());
  }

  static const std::string name() { return "GetBalance"; }

  common::cmd::Error doProcess(
      const hub::rpc::GetBalanceRequest* request,
      hub::rpc::GetBalanceReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_GET_BALANCE_H_
