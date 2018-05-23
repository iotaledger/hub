/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */


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

/// UserBalanceSubscription command.
/// Constructor
/// @param[in] hub::rpc::UserBalanceSubscriptionRequest
/// @param[in] hub::rpc::ServerWriterInterface
class UserBalanceSubscription
    : public Command<hub::rpc::UserBalanceSubscriptionRequest,
                     grpc::ServerWriterInterface<hub::rpc::UserBalanceEvent>> {
 public:
  using Command<
      hub::rpc::UserBalanceSubscriptionRequest,
      grpc::ServerWriterInterface<hub::rpc::UserBalanceEvent>>::Command;

  const std::string name() override { return "UserBalanceSubscription"; }

  virtual std::vector<db::UserBalanceEvent> getAccountBalances(
      std::chrono::system_clock::time_point lastCheck);

 protected:
  grpc::Status doProcess(
      const hub::rpc::UserBalanceSubscriptionRequest* request,
      grpc::ServerWriterInterface<hub::rpc::UserBalanceEvent>*
          writer) noexcept override;
};
}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_USER_BALANCE_SUBSCRIPTION_H_
