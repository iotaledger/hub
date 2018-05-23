/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */


#ifndef HUB_COMMANDS_GET_USER_HISTORY_H_
#define HUB_COMMANDS_GET_USER_HISTORY_H_

#include <string>

#include "hub/commands/command.h"

namespace hub {
namespace rpc {
class GetUserHistoryRequest;
class GetUserHistoryReply;
}  // namespace rpc

namespace cmd {

/// GetUserHistory command. Gets the history of transactions for a user.
/// Returns ???
/// Constructor
/// @param[in] hub::rpc::GetUserHistoryRequest
/// @param[in] hub::rpc::GetUserHistoryReply
class GetUserHistory : public Command<hub::rpc::GetUserHistoryRequest,
                                      hub::rpc::GetUserHistoryReply> {
 public:
  using Command<hub::rpc::GetUserHistoryRequest,
                hub::rpc::GetUserHistoryReply>::Command;

  const std::string name() override { return "GetUserHistory"; }

 protected:
  grpc::Status doProcess(
      const hub::rpc::GetUserHistoryRequest* request,
      hub::rpc::GetUserHistoryReply* response) noexcept override;
};
}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_GET_USER_HISTORY_H_
