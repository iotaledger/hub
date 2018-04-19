#ifndef __HUB_COMMANDS_CREATE_USER_H_
#define __HUB_COMMANDS_CREATE_USER_H_

#include "command.h"

namespace iota {
namespace rpc {
class CreateUserRequest;
class CreateUserReply;
}  // namespace rpc

namespace cmd {

class CreateUser
    : public Command<iota::rpc::CreateUserRequest, iota::rpc::CreateUserReply> {
 public:
  using Command<iota::rpc::CreateUserRequest,
                iota::rpc::CreateUserReply>::Command;

  grpc::Status doProcess(
      const iota::rpc::CreateUserRequest* request,
      iota::rpc::CreateUserReply* response) noexcept override;

  const std::string name() override { return "CreateUser"; }
};
}  // namespace cmd
}  // namespace iota

#endif /* __HUB_COMMANDS_CREATE_USER_H_ */
