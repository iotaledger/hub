/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_SWEEP_INFO_H_
#define HUB_COMMANDS_SWEEP_INFO_H_

#include <string>

#include "common/commands/command.h"
#include "events.h"

namespace hub {

namespace cmd {

typedef struct SweepInfoRequest {
  bool requestByUuid;
  std::string bundleHash;
  std::string uuid;
} SweepInfoRequest;

/// Gets the history of transactions for a user.
/// @param[in] SweepInfoRequest
/// @param[in] SweepEvent
class SweepInfo : public common::Command<SweepInfoRequest, SweepEvent> {
 public:
  using Command<SweepInfoRequest, SweepEvent>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new SweepInfo(std::make_shared<common::ClientSession>()));
  }

  static const std::string name() { return "SweepInfo"; }

  common::cmd::Error doProcess(const SweepInfoRequest* request,
                               SweepEvent* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_SWEEP_INFO_H_
