/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_CREATE_USER_H_
#define HUB_COMMANDS_CREATE_USER_H_

#include "common/commands/command.h"

#include <string>

namespace hub {
namespace cmd {

typedef struct CreateUserRequest {
  std::string userId;

} CreateUserRequest;

typedef struct CreateUserReply {
  std::string userId;

} CreateUserReply;

/// Creates a new user with a specific id.
/// @param[in] CreateUserRequest
/// @param[in] CreateUserReply
class CreateUser : public common::Command<CreateUserRequest, CreateUserReply> {
 public:
  using Command<CreateUserRequest, CreateUserReply>::Command;

  static std::shared_ptr<ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new CreateUser(std::make_shared<common::ClientSession>()));
  }

  static const std::string name() { return "CreateUser"; }

  common::cmd::Error doProcess(const CreateUserRequest* request,
                               CreateUserReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_CREATE_USER_H_
