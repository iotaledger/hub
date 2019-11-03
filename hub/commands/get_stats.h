/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_GET_STATS_H_
#define HUB_COMMANDS_GET_STATS_H_

#include <string>

#include "common/commands/command.h"

namespace hub {
namespace cmd {

typedef struct GetStatsRequest {
} GetStatsRequest;

typedef struct GetStatsReply {
  uint64_t totalBalance;
} GetStatsReply;

/// Gets statistics about the Hub's current state
/// @param[in] GetStatsRequest
/// @param[in] GetStatsReply
class GetStats : public common::Command<GetStatsRequest, GetStatsReply> {
 public:
  using Command<GetStatsRequest, GetStatsReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new GetStats(std::make_shared<common::ClientSession>()));
  }

  static const std::string name() { return "GetStats"; }

  common::cmd::Error doProcess(const GetStatsRequest* request,
                               GetStatsReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_GET_STATS_H_
