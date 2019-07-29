/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_GET_STATS_H_
#define HUB_COMMANDS_GET_STATS_H_

#include <string>

#include "common/command.h"

namespace hub {
namespace rpc {
class GetStatsRequest;
class GetStatsReply;
}  // namespace rpc

namespace cmd {

/// Gets statistics about the Hub's current state
/// @param[in] hub::rpc::GetStatsRequest
/// @param[in] hub::rpc::GetStatsReply
class GetStats : public common::Command<hub::rpc::GetStatsRequest,
                                        hub::rpc::GetStatsReply> {
 public:
  using Command<hub::rpc::GetStatsRequest, hub::rpc::GetStatsReply>::Command;

  const std::string name() override { return "GetStats"; }

  grpc::Status doProcess(const hub::rpc::GetStatsRequest* request,
                         hub::rpc::GetStatsReply* response) noexcept override;
};
}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_GET_STATS_H_
