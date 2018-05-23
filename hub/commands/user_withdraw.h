/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */


#ifndef HUB_COMMANDS_USER_WITHDRAW_H_
#define HUB_COMMANDS_USER_WITHDRAW_H_

#include <string>

#include "hub/commands/command.h"

namespace hub {
namespace rpc {
class UserWithdrawRequest;
class UserWithdrawReply;
}  // namespace rpc

namespace cmd {

/// Process a withdrawal command for a user.
/// @param[in] hub::rpc::UserWithdrawRequest
/// @param[in] hub::rpc::UserWithdrawReply
class UserWithdraw : public Command<hub::rpc::UserWithdrawRequest,
                                    hub::rpc::UserWithdrawReply> {
 public:
  using Command<hub::rpc::UserWithdrawRequest,
                hub::rpc::UserWithdrawReply>::Command;

  const std::string name() override { return "UserWithdraw"; }

  grpc::Status doProcess(
      const hub::rpc::UserWithdrawRequest* request,
      hub::rpc::UserWithdrawReply* response) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_USER_WITHDRAW_H_
