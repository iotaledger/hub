/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_BALANCE_SUBSCRIPTION_H_
#define HUB_COMMANDS_BALANCE_SUBSCRIPTION_H_

#include <string>
#include <vector>

#include <grpc++/support/sync_stream.h>

#include "hub/commands/command.h"
#include "hub/db/helper.h"

namespace hub {
namespace rpc {
class BalanceSubscriptionRequest;
class BalanceEvent;
}  // namespace rpc

namespace cmd {

/// @param[in] hub::rpc::BalanceSubscriptionRequest
/// @param[in] hub::rpc::ServerWriterInterface
/// Collects records about balance actions to and from user addresses
/// and hub's addresses as well (Depsoits/Withdrawals/Hub address actions)
class BalanceSubscription
    : public Command<hub::rpc::BalanceSubscriptionRequest,
                     grpc::ServerWriterInterface<hub::rpc::BalanceEvent>> {
 public:
  using Command<hub::rpc::BalanceSubscriptionRequest,
                grpc::ServerWriterInterface<hub::rpc::BalanceEvent>>::Command;

  const std::string name() override { return "BalanceSubscription"; }

  virtual std::vector<db::UserAccountBalanceEvent>
  getAllUsersAccountBalancesSinceTimePoint(
      std::chrono::system_clock::time_point lastCheck);

  virtual std::vector<db::UserAddressBalanceEvent>
  getAllUserAddressesBalancesSinceTimePoint(
      std::chrono::system_clock::time_point lastCheck);

  virtual std::vector<db::HubAddressBalanceEvent>
  getAllHubAddressesBalancesSinceTimePoint(
      std::chrono::system_clock::time_point lastCheck);

  grpc::Status doProcess(const hub::rpc::BalanceSubscriptionRequest* request,
                         grpc::ServerWriterInterface<hub::rpc::BalanceEvent>*
                             writer) noexcept override;
};
}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_BALANCE_SUBSCRIPTION_H_
