/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_BALANCE_SUBSCRIPTION_H_
#define HUB_COMMANDS_BALANCE_SUBSCRIPTION_H_

#include <memory>
#include <string>
#include <vector>

#include <grpc++/support/sync_stream.h>
#include "events.h"

#include "common/commands/command.h"
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
    : public common::Command<
          hub::rpc::BalanceSubscriptionRequest,
          grpc::ServerWriterInterface<hub::rpc::BalanceEvent>> {
 public:
  using Command<hub::rpc::BalanceSubscriptionRequest,
                grpc::ServerWriterInterface<hub::rpc::BalanceEvent>>::Command;

  static const std::string name() { return "BalanceSubscription"; }

  static std::shared_ptr<ICommand> create() {
    return std::shared_ptr<common::ICommand>(new BalanceSubscription());
  }

  virtual std::vector<db::UserAccountBalanceEvent>
  getAllUsersAccountBalancesSinceTimePoint(
      std::chrono::system_clock::time_point lastCheck);

  virtual std::vector<db::UserAddressBalanceEvent>
  getAllUserAddressesBalancesSinceTimePoint(
      std::chrono::system_clock::time_point lastCheck);

  virtual std::vector<db::HubAddressBalanceEvent>
  getAllHubAddressesBalancesSinceTimePoint(
      std::chrono::system_clock::time_point lastCheck);

  common::cmd::Error doProcess(
      const hub::rpc::BalanceSubscriptionRequest* request,
      grpc::ServerWriterInterface<hub::rpc::BalanceEvent>*
          writer) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_BALANCE_SUBSCRIPTION_H_
