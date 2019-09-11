/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_RECOVER_FUNDS_H_
#define HUB_COMMANDS_RECOVER_FUNDS_H_

#include <string>

#include "common/command.h"
#include "cppclient/api.h"

namespace hub {
namespace rpc {
class RecoverFundsRequest;
class RecoverFundsReply;
}  // namespace rpc

namespace cmd {

/// Recover funds from an already spent address into an output address
/// @param[in] hub::rpc::RecoverFundsRequest
/// @param[in] hub::rpc::RecoverFundsReply
class RecoverFunds : public common::Command<hub::rpc::RecoverFundsRequest,
                                            hub::rpc::RecoverFundsReply> {
 public:
  using Command<hub::rpc::RecoverFundsRequest,
                hub::rpc::RecoverFundsReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(new RecoverFunds());
  }

  explicit RecoverFunds(std::shared_ptr<common::ClientSession> session,
                        std::shared_ptr<cppclient::IotaAPI> api)
      : Command(std::move(session)), _api(std::move(api)) {}

  static const std::string name() { return "RecoverFunds"; }

  grpc::Status doProcess(
      const hub::rpc::RecoverFundsRequest* request,
      hub::rpc::RecoverFundsReply* response) noexcept override;

  std::string doProcess(
      const boost::property_tree::ptree& request) noexcept override;

 private:
  std::shared_ptr<cppclient::IotaAPI> _api;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_RECOVER_FUNDS_H_
