// Copyright 2018 IOTA Foundation

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

class GetBalance
    : public Command<hub::rpc::GetBalanceRequest, hub::rpc::GetBalanceReply> {
 public:
  using Command<hub::rpc::GetBalanceRequest,
                hub::rpc::GetBalanceReply>::Command;

  grpc::Status doProcess(const hub::rpc::GetBalanceRequest* request,
                         hub::rpc::GetBalanceReply* response) noexcept override;

  const std::string name() override { return "GetBalance"; }
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_GET_BALANCE_H_
