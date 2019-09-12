/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_USER_WITHDRAW_H_
#define HUB_COMMANDS_USER_WITHDRAW_H_

#include <memory>
#include <string>
#include <utility>

#include "common/command.h"
#include "common/stats/session.h"
#include "cppclient/api.h"

namespace hub {
namespace rpc {
class UserWithdrawRequest;
class UserWithdrawReply;
}  // namespace rpc

namespace cmd {

/// Process a withdrawal command for a user.
/// @param[in] hub::rpc::UserWithdrawRequest
/// @param[in] hub::rpc::UserWithdrawReply
class UserWithdraw : public common::Command<hub::rpc::UserWithdrawRequest,
                                            hub::rpc::UserWithdrawReply> {
 public:
  using Command<hub::rpc::UserWithdrawRequest,
                hub::rpc::UserWithdrawReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(new UserWithdraw());
  }

  explicit UserWithdraw(std::shared_ptr<common::ClientSession> session,
                        std::shared_ptr<cppclient::IotaAPI> api)
      : Command(std::move(session)), _api(std::move(api)) {}

  static const std::string name() { return "UserWithdraw"; }

  grpc::Status doProcess(
      const hub::rpc::UserWithdrawRequest* request,
      hub::rpc::UserWithdrawReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;

 private:
  std::shared_ptr<cppclient::IotaAPI> _api;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_USER_WITHDRAW_H_
