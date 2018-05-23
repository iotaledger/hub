/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_GET_BALANCE_H_
#define HUB_COMMANDS_GET_BALANCE_H_

#include <string>

#include "hub/commands/command.h"

namespace hub {
namespace rpc {
class GetBalanceRequest;
class GetBalanceReply;
}  // namespace rpc

namespace cmd {

/// GetBalance command. Gets the current balance for a user with a specific id.
/// Returns ???
/// Constructor
/// @param[in] hub::rpc::GetBalanceRequest
/// @param[in] hub::rpc::GetBalanceReply
class GetBalance
    : public Command<hub::rpc::GetBalanceRequest, hub::rpc::GetBalanceReply> {
 public:
  using Command<hub::rpc::GetBalanceRequest,
                hub::rpc::GetBalanceReply>::Command;

  const std::string name() override { return "GetBalance"; }

  protected:
   grpc::Status doProcess(const hub::rpc::GetBalanceRequest* request,
                          hub::rpc::GetBalanceReply* response) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_GET_BALANCE_H_
