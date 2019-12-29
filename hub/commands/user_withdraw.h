/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_USER_WITHDRAW_H_
#define HUB_COMMANDS_USER_WITHDRAW_H_

#include <string>

#include "cppclient/api.h"

#include "common/commands/command.h"

namespace hub {
namespace cmd {

typedef struct UserWithdrawRequest {
  std::string userId;
  uint64_t amount;
  bool validateChecksum;
  std::string payoutAddress;
  std::string tag;
} UserWithdrawRequest;
typedef struct UserWithdrawReply {
  std::string uuid;
} UserWithdrawReply;

/// Process a withdrawal command for a user.
/// @param[in] UserWithdrawRequest
/// @param[in] rpc::UserWithdrawReply
class UserWithdraw
    : public common::Command<UserWithdrawRequest, UserWithdrawReply> {
 public:
  using Command<UserWithdrawRequest, UserWithdrawReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new UserWithdraw(std::make_shared<common::ClientSession>()));
  }

  explicit UserWithdraw(std::shared_ptr<common::ClientSession> session,
                        std::shared_ptr<cppclient::IotaAPI> api)
      : Command(std::move(session)), _api(std::move(api)) {}

  static const std::string name() { return "UserWithdraw"; }

  common::cmd::Error doProcess(const UserWithdrawRequest* request,
                               UserWithdrawReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;

  virtual bool needApi() const override { return true; }

  virtual void setApi(std::shared_ptr<cppclient::IotaAPI> api) { _api = api; }

 private:
  std::shared_ptr<cppclient::IotaAPI> _api;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_USER_WITHDRAW_H_
