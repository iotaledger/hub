/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_WAS_ADDRESS_SPENT_FROM_H_
#define HUB_COMMANDS_WAS_ADDRESS_SPENT_FROM_H_

#include <string>

#include "cppclient/api.h"

#include "common/commands/command.h"

namespace hub {

namespace cmd {

typedef struct WasAddressSpentFromRequest {
  std::string address;
  bool validateChecksum;
} WasAddressSpentFromRequest;

typedef struct WasAddressSpentFromReply {
  bool wasAddressSpentFrom;
} WasAddressSpentFromReply;

/// Returns whether or not an address was spent.
/// @param[in] WasAddressSpentFromRequest
/// @param[in] WasAddressSpentFromReply
class WasAddressSpentFrom : public common::Command<WasAddressSpentFromRequest,
                                                   WasAddressSpentFromReply> {
 public:
  using Command<WasAddressSpentFromRequest, WasAddressSpentFromReply>::Command;

  static std::shared_ptr<ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new WasAddressSpentFrom(std::make_shared<common::ClientSession>()));
  }

  explicit WasAddressSpentFrom(std::shared_ptr<common::ClientSession> session,
                               std::shared_ptr<cppclient::IotaAPI> api)
      : Command(std::move(session)), _api(std::move(api)) {}

  static const std::string name() { return "WasAddressSpentFrom"; }

  common::cmd::Error doProcess(
      const WasAddressSpentFromRequest* request,
      WasAddressSpentFromReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;

  virtual bool needApi() const override { return true; }

  virtual void setApi(std::shared_ptr<cppclient::IotaAPI> api) { _api = api; }

 private:
  std::shared_ptr<cppclient::IotaAPI> _api;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_WAS_ADDRESS_SPENT_FROM_H_
