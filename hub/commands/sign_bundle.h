/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_SIGN_BUNDLE_H_
#define HUB_COMMANDS_SIGN_BUNDLE_H_

#include <string>

#include "common/commands/command.h"

namespace hub {
namespace rpc {
class SignBundleRequest;
class SignBundleReply;
}  // namespace rpc

namespace cmd {

/// Gets information on an address
/// @param[in] hub::rpc::SignBundleRequest
/// @param[in] hub::rpc::SignBundleReply
class SignBundle : public common::Command<hub::rpc::SignBundleRequest,
                                          hub::rpc::SignBundleReply> {
 public:
  using Command<hub::rpc::SignBundleRequest,
                hub::rpc::SignBundleReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(new SignBundle());
  }

  static const std::string name() { return "SignBundle"; }

  common::cmd::Error doProcess(
      const hub::rpc::SignBundleRequest* request,
      hub::rpc::SignBundleReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_SIGN_BUNDLE_H_
