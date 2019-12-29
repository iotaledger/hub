/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_SWEEP_DETAIL_H_
#define HUB_COMMANDS_SWEEP_DETAIL_H_

#include <string>

#include "common/commands/command.h"

namespace hub {

namespace cmd {

typedef struct SweepDetailRequest {
  std::string bundleHash;
} SweepDetailRequest;
typedef struct SweepDetailReply {
  bool confirmed;
  std::vector<std::string> trytes;
  std::vector<std::string> tailHashes;

} SweepDetailReply;

/// Gets the history of transactions for a user.
/// @param[in] SweepInfoRequest
/// @param[in] SweepEvent
class SweepDetail
    : public common::Command<SweepDetailRequest, SweepDetailReply> {
 public:
  using Command<SweepDetailRequest, SweepDetailReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new SweepDetail(std::make_shared<common::ClientSession>()));
  }

  static const std::string name() { return "SweepDetail"; }

  common::cmd::Error doProcess(const SweepDetailRequest* request,
                               SweepDetailReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_SWEEP_DETAIL_H_
