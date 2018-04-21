#ifndef __HUB_COMMANDS_CREATE_USER_H_
#define __HUB_COMMANDS_CREATE_USER_H_

#include "command.h"

namespace hub {
namespace rpc {
class CreateUserRequest;
class CreateUserReply;
}  // namespace rpc

namespace cmd {

class CreateUser
    : public Command<hub::rpc::CreateUserRequest, hub::rpc::CreateUserReply> {
 public:
  using Command<hub::rpc::CreateUserRequest,
                hub::rpc::CreateUserReply>::Command;

  grpc::Status doProcess(
      const hub::rpc::CreateUserRequest* request,
      hub::rpc::CreateUserReply* response) noexcept override;

  const std::string name() override { return "CreateUser"; }
};
}  // namespace cmd
}  // namespace hub

#endif /* __HUB_COMMANDS_CREATE_USER_H_ */
