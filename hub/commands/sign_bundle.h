/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_SIGN_BUNDLE_H_
#define HUB_COMMANDS_SIGN_BUNDLE_H_

#include <string>

#include "hub/commands/command.h"

namespace hub {
namespace rpc {
class SignBundleRequest;
class SignBundleReply;
}  // namespace rpc

namespace cmd {

/// Gets information on an address
/// @param[in] hub::rpc::SignBundleRequest
/// @param[in] hub::rpc::SignBundleReply
class SignBundle : public Command<hub::rpc::SignBundleRequest,
                                         hub::rpc::SignBundleReply> {
 public:
  using Command<hub::rpc::SignBundleRequest,
                hub::rpc::SignBundleReply>::Command;

  const std::string name() override { return "SignBundle"; }

  grpc::Status doProcess(
      const hub::rpc::SignBundleRequest* request,
      hub::rpc::SignBundleReply* response) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_SIGN_BUNDLE_H_
