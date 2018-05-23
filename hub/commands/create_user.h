/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_CREATE_USER_H_
#define HUB_COMMANDS_CREATE_USER_H_

#include <string>

#include "hub/commands/command.h"

namespace hub {
namespace rpc {
class CreateUserRequest;
class CreateUserReply;
}  // namespace rpc

namespace cmd {

/// CreateUser command. Creates a new user with a specific id. Returns ???
/// Constructor
/// @param[in] hub::rpc::CreateUserRequest
/// @param[in] hub::rpc::CreateUserReply
class CreateUser
    : public Command<hub::rpc::CreateUserRequest, hub::rpc::CreateUserReply> {
 public:
  using Command<hub::rpc::CreateUserRequest,
                hub::rpc::CreateUserReply>::Command;

  const std::string name() override { return "CreateUser"; }

  protected:
   grpc::Status doProcess(const hub::rpc::CreateUserRequest* request,
                          hub::rpc::CreateUserReply* response) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_CREATE_USER_H_
