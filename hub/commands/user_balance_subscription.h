// Copyright 2018 IOTA Foundation

#ifndef HUB_COMMANDS_USER_BALANCE_SUBSCRIPTION_H_
#define HUB_COMMANDS_USER_BALANCE_SUBSCRIPTION_H_

#include <string>
#include <vector>

#include <grpc++/support/sync_stream.h>

#include "hub/commands/command.h"
#include "hub/db/helper.h"

namespace hub {
namespace rpc {
class UserBalanceSubscriptionRequest;
class UserBalanceEvent;
}  // namespace rpc

namespace cmd {

class UserBalanceSubscription
    : public Command<hub::rpc::UserBalanceSubscriptionRequest,
                     grpc::ServerWriterInterface<hub::rpc::UserBalanceEvent>> {
 public:
  using Command<
      hub::rpc::UserBalanceSubscriptionRequest,
      grpc::ServerWriterInterface<hub::rpc::UserBalanceEvent>>::Command;

  grpc::Status doProcess(
      const hub::rpc::UserBalanceSubscriptionRequest* request,
      grpc::ServerWriterInterface<hub::rpc::UserBalanceEvent>*
          writer) noexcept override;

  const std::string name() override { return "UserBalanceSubscription"; }

  virtual std::vector<db::UserBalanceEvent> getAccountBalances(
      std::chrono::system_clock::time_point lastCheck);
};
}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_USER_BALANCE_SUBSCRIPTION_H_
