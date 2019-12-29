/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_WAS_WITHDRAWAL_CANCELLED_H_
#define HUB_COMMANDS_WAS_WITHDRAWAL_CANCELLED_H_

#include <string>

#include "common/commands/command.h"

namespace hub {
namespace cmd {

typedef struct WasWithdrawalCancelledRequest {
  std::string uuid;
} WasWithdrawalCancelledRequest;

typedef struct WasWithdrawalCancelledReply {
  bool wasWihdrawalCancelled;
} WasWithdrawalCancelledReply;

/// Returns true if withdrawal was cancelled
/// @param[in] request - a WasWithdrawalCancelledRequest request
/// @param[in] response - a WasWithdrawalCancelledResponse response
class WasWithdrawalCancelled
    : public common::Command<WasWithdrawalCancelledRequest,
                             WasWithdrawalCancelledReply> {
 public:
  using Command<WasWithdrawalCancelledRequest,
                WasWithdrawalCancelledReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new WasWithdrawalCancelled(std::make_shared<common::ClientSession>()));
  }

  static const std::string name() { return "WasWithdrawalCancelled"; }

  common::cmd::Error doProcess(
      const WasWithdrawalCancelledRequest* request,
      WasWithdrawalCancelledReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_WAS_WITHDRAWAL_CANCELLED_H_
