/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_SWEEP_INFO_H_
#define HUB_COMMANDS_SWEEP_INFO_H_

#include <string>

#include "common/command.h"

namespace hub {
namespace rpc {
class SweepInfoRequest;
class SweepEvent;
}  // namespace rpc

namespace cmd {

/// Gets the history of transactions for a user.
/// @param[in] hub::rpc::SweepInfoRequest
/// @param[in] hub::rpc::SweepEvent
class SweepInfo : public common::Command<hub::rpc::SweepInfoRequest,
                                              hub::rpc::SweepEvent> {
 public:
  using Command<hub::rpc::SweepInfoRequest, hub::rpc::SweepEvent>::Command;

  const std::string name() override { return "SweepInfo"; }

  grpc::Status doProcess(const hub::rpc::SweepInfoRequest* request,
                         hub::rpc::SweepEvent* response) noexcept override;
};
}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_SWEEP_INFO_H_
