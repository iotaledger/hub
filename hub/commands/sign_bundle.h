/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_SIGN_BUNDLE_H_
#define HUB_COMMANDS_SIGN_BUNDLE_H_

#include <string>

#include "common/commands/command.h"

DECLARE_bool(SignBundle_enabled);

namespace hub {

namespace cmd {

typedef struct SignBundleRequest {
  bool validateChecksum;
  std::string address;
  std::string authenticationToken;
  std::string bundleHash;
} SignBundleRequest;
typedef struct SignBundleReply {
  std::string signature;
} SignBundleReply;

/// Gets information on an address
/// @param[in] SignBundleRequest
/// @param[in] SignBundleReply
class SignBundle : public common::Command<SignBundleRequest, SignBundleReply> {
 public:
  using Command<SignBundleRequest, SignBundleReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new SignBundle(std::make_shared<common::ClientSession>()));
  }

  static const std::string name() { return "SignBundle"; }

  common::cmd::Error doProcess(const SignBundleRequest* request,
                               SignBundleReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_SIGN_BUNDLE_H_
