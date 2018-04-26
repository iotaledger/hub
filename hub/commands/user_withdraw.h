// Copyright 2018 IOTA Foundation

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

class UserWithdraw : public Command<hub::rpc::UserWithdrawRequest,
                                    hub::rpc::UserWithdrawReply> {
 public:
  using Command<hub::rpc::UserWithdrawRequest,
                hub::rpc::UserWithdrawReply>::Command;

  grpc::Status doProcess(
      const hub::rpc::UserWithdrawRequest* request,
      hub::rpc::UserWithdrawReply* response) noexcept override;

  const std::string name() override { return "UserWithdraw"; }
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_USER_WITHDRAW_H_
