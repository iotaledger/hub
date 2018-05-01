// Copyright 2018 IOTA Foundation

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

class GetUserHistory : public Command<hub::rpc::GetUserHistoryRequest,
                                      hub::rpc::GetUserHistoryReply> {
 public:
  using Command<hub::rpc::GetUserHistoryRequest,
                hub::rpc::GetUserHistoryReply>::Command;

  grpc::Status doProcess(
      const hub::rpc::GetUserHistoryRequest* request,
      hub::rpc::GetUserHistoryReply* response) noexcept override;

  const std::string name() override { return "GetUserHistory"; }
};
}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_GET_USER_HISTORY_H_
