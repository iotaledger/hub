/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_GET_USER_HISTORY_H_
#define HUB_COMMANDS_GET_USER_HISTORY_H_

#include <string>
#include <vector>

#include "common/commands/command.h"
#include "hub/commands/events.h"

namespace hub {
namespace cmd {

/// Gets the history of transactions for a user.
/// @param[in] GetUserHistoryRequest
/// @param[in] GetUserHistoryReply

typedef struct GetUserHistoryRequest {
  std::string userId;
  uint64_t newerThan;
} GetUserHistoryRequest;

typedef struct GetUserHistoryReply {
  std::vector<UserAccountBalanceEvent> events;
} GetUserHistoryReply;

class GetUserHistory
    : public common::Command<GetUserHistoryRequest, GetUserHistoryReply> {
 public:
  using Command<GetUserHistoryRequest, GetUserHistoryReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new GetUserHistory(std::make_shared<common::ClientSession>()));
  }

  static const std::string name() { return "GetUserHistory"; }

  common::cmd::Error doProcess(const GetUserHistoryRequest* request,
                               GetUserHistoryReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_GET_USER_HISTORY_H_
