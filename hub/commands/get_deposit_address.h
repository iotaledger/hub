#ifndef __HUB_COMMANDS_GET_DEPOSIT_ADDRESS_H_
#define __HUB_COMMANDS_GET_DEPOSIT_ADDRESS_H_

#include "command.h"

namespace iota {
namespace rpc {
class GetDepositAddressRequest;
class GetDepositAddressReply;
}  // namespace rpc

namespace cmd {

class GetDepositAddress
    : public Command<iota::rpc::GetDepositAddressRequest, iota::rpc::GetDepositAddressReply> {
 public:
  using Command<iota::rpc::GetDepositAddressRequest,
                iota::rpc::GetDepositAddressReply>::Command;

  grpc::Status doProcess(
      const iota::rpc::GetDepositAddressRequest* request,
      iota::rpc::GetDepositAddressReply* response) noexcept override;

  const std::string name() override { return "GetDepositAddress"; }
};
}  // namespace cmd
}  // namespace iota

#endif /* __HUB_COMMANDS_GET_DEPOSIT_ADDRESS_H_ */
