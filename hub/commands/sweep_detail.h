/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_SWEEP_DETAIL_H_
#define HUB_COMMANDS_SWEEP_DETAIL_H_

#include <string>

#include "common/commands/command.h"

namespace hub {
namespace rpc {
class SweepDetailRequest;
class SweepDetailReply;
}  // namespace rpc

namespace cmd {

/// Gets the history of transactions for a user.
/// @param[in] hub::rpc::SweepInfoRequest
/// @param[in] hub::rpc::SweepEvent
class SweepDetail : public common::Command<hub::rpc::SweepDetailRequest,
                                           hub::rpc::SweepDetailReply> {
 public:
  using Command<hub::rpc::SweepDetailRequest,
                hub::rpc::SweepDetailReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(new SweepDetail());
  }

  static const std::string name() { return "SweepDetail"; }

  common::cmd::Error doProcess(
      const hub::rpc::SweepDetailRequest* request,
      hub::rpc::SweepDetailReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_SWEEP_DETAIL_H_
