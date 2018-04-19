#ifndef __HUB_COMMANDS_GET_BALANCE_H_
#define __HUB_COMMANDS_GET_BALANCE_H_

#include "command.h"

namespace iota {
namespace rpc {
class GetBalanceRequest;
class GetBalanceReply;
}  // namespace rpc

namespace cmd {

class GetBalance
    : public Command<iota::rpc::GetBalanceRequest, iota::rpc::GetBalanceReply> {
 public:
  using Command<iota::rpc::GetBalanceRequest,
                iota::rpc::GetBalanceReply>::Command;

  grpc::Status doProcess(
      const iota::rpc::GetBalanceRequest* request,
      iota::rpc::GetBalanceReply* response) noexcept override;

  const std::string name() override { return "GetBalance"; }
};
}  // namespace cmd
}  // namespace iota

#endif /* __HUB_COMMANDS_GET_BALANCE_H_ */
