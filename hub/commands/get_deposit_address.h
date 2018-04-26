// Copyright 2018 IOTA Foundation

#ifndef HUB_COMMANDS_GET_DEPOSIT_ADDRESS_H_
#define HUB_COMMANDS_GET_DEPOSIT_ADDRESS_H_

#include <string>

#include "hub/commands/command.h"

namespace hub {
namespace rpc {
class GetDepositAddressRequest;
class GetDepositAddressReply;
}  // namespace rpc

namespace cmd {

class GetDepositAddress : public Command<hub::rpc::GetDepositAddressRequest,
                                         hub::rpc::GetDepositAddressReply> {
 public:
  using Command<hub::rpc::GetDepositAddressRequest,
                hub::rpc::GetDepositAddressReply>::Command;

  grpc::Status doProcess(
      const hub::rpc::GetDepositAddressRequest* request,
      hub::rpc::GetDepositAddressReply* response) noexcept override;

  const std::string name() override { return "GetDepositAddress"; }
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_GET_DEPOSIT_ADDRESS_H_
