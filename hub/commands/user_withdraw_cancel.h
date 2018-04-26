// Copyright 2018 IOTA Foundation

#ifndef HUB_COMMANDS_USER_WITHDRAW_CANCEL_H_
#define HUB_COMMANDS_USER_WITHDRAW_CANCEL_H_

#include <string>

#include "hub/commands/command.h"

namespace hub {
namespace rpc {
class UserWithdrawCancelRequest;
class UserWithdrawCancelReply;
}  // namespace rpc

namespace cmd {

class UserWithdrawCancel : public Command<hub::rpc::UserWithdrawCancelRequest,
                                          hub::rpc::UserWithdrawCancelReply> {
 public:
  using Command<hub::rpc::UserWithdrawCancelRequest,
                hub::rpc::UserWithdrawCancelReply>::Command;

  grpc::Status doProcess(
      const hub::rpc::UserWithdrawCancelRequest* request,
      hub::rpc::UserWithdrawCancelReply* response) noexcept override;

  const std::string name() override { return "UserWithdrawCancel"; }
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_USER_WITHDRAW_CANCEL_H_
