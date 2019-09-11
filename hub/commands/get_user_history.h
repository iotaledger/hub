/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_GET_USER_HISTORY_H_
#define HUB_COMMANDS_GET_USER_HISTORY_H_

#include <string>

#include "common/command.h"

namespace hub {
namespace rpc {
class GetUserHistoryRequest;
class GetUserHistoryReply;
}  // namespace rpc

namespace cmd {

/// Gets the history of transactions for a user.
/// @param[in] hub::rpc::GetUserHistoryRequest
/// @param[in] hub::rpc::GetUserHistoryReply
class GetUserHistory : public common::Command<hub::rpc::GetUserHistoryRequest,
                                              hub::rpc::GetUserHistoryReply> {
 public:
  using Command<hub::rpc::GetUserHistoryRequest,
                hub::rpc::GetUserHistoryReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(new GetUserHistory());
  }

  static const std::string name() { return "GetUserHistory"; }

  grpc::Status doProcess(
      const hub::rpc::GetUserHistoryRequest* request,
      hub::rpc::GetUserHistoryReply* response) noexcept override;

  std::string doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_GET_USER_HISTORY_H_
