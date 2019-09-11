/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_WAS_ADDRESS_SPENT_FROM_H_
#define HUB_COMMANDS_WAS_ADDRESS_SPENT_FROM_H_

#include <memory>
#include <string>
#include <utility>

#include "common/command.h"
#include "common/stats/session.h"
#include "cppclient/api.h"

namespace hub {
namespace rpc {
class WasAddressSpentFromRequest;
class WasAddressSpentFromReply;
}  // namespace rpc

namespace cmd {

/// Returns whether or not an address was spent.
/// @param[in] hub::rpc::WasAddressSpentFromRequest
/// @param[in] hub::rpc::WasAddressSpentFromReply
class WasAddressSpentFrom
    : public common::Command<hub::rpc::WasAddressSpentFromRequest,
                             hub::rpc::WasAddressSpentFromReply> {
 public:
  using Command<hub::rpc::WasAddressSpentFromRequest,
                hub::rpc::WasAddressSpentFromReply>::Command;

  static std::shared_ptr<ICommand> create() {
    return std::shared_ptr<common::ICommand>(new WasAddressSpentFrom());
  }

  explicit WasAddressSpentFrom(std::shared_ptr<common::ClientSession> session,
                               std::shared_ptr<cppclient::IotaAPI> api)
      : Command(std::move(session)), _api(std::move(api)) {}

  static const std::string name() { return "WasAddressSpentFrom"; }

  grpc::Status doProcess(
      const hub::rpc::WasAddressSpentFromRequest* request,
      hub::rpc::WasAddressSpentFromReply* response) noexcept override;

  std::string doProcess(
      const boost::property_tree::ptree& request) noexcept override;

 private:
  std::shared_ptr<cppclient::IotaAPI> _api;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_WAS_ADDRESS_SPENT_FROM_H_
