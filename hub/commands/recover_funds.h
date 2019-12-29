/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_RECOVER_FUNDS_H_
#define HUB_COMMANDS_RECOVER_FUNDS_H_

#include <string>

#include "common/commands/command.h"
#include "cppclient/api.h"

DECLARE_bool(RecoverFunds_enabled);

namespace hub {
namespace cmd {

typedef struct RecoverFundsRequest {
  std::string userId;
  std::string address;
  std::string payoutAddress;
  bool validateChecksum;
} RecoverFundsRequest;
typedef struct RecoverFundsReply {
} RecoverFundsReply;

/// Recover funds from an already spent address into an output address
/// @param[in] RecoverFundsRequest
/// @param[in] RecoverFundsReply
class RecoverFunds
    : public common::Command<RecoverFundsRequest, RecoverFundsReply> {
 public:
  using Command<RecoverFundsRequest, RecoverFundsReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new RecoverFunds(std::make_shared<common::ClientSession>()));
  }

  explicit RecoverFunds(std::shared_ptr<common::ClientSession> session,
                        std::shared_ptr<cppclient::IotaAPI> api)
      : Command(std::move(session)), _api(std::move(api)) {}

  static const std::string name() { return "RecoverFunds"; }

  common::cmd::Error doProcess(const RecoverFundsRequest* request,
                               RecoverFundsReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;

  virtual bool needApi() const override { return true; }

  virtual void setApi(std::shared_ptr<cppclient::IotaAPI> api) { _api = api; }

 private:
  std::shared_ptr<cppclient::IotaAPI> _api;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_RECOVER_FUNDS_H_
