/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_BALANCE_SUBSCRIPTION_H_
#define HUB_COMMANDS_BALANCE_SUBSCRIPTION_H_

#include <string>
#include <vector>

#include "common/commands/command.h"
#include "events.h"
#include "hub/db/helper.h"

namespace hub {

namespace cmd {

typedef struct BalanceSubscriptionRequest {
  uint64_t newerThan;
} BalanceSubscriptionRequest;

/// @param[in] BalanceSubscriptionRequest
/// @param[out] std::vector<BalanceEvent> events
/// Collects records about balance actions to and from user addresses
/// and hub's addresses as well (Depsoits/Withdrawals/Hub address actions)
class BalanceSubscription : public common::Command<BalanceSubscriptionRequest,
                                                   std::vector<BalanceEvent>> {
 public:
  using Command<BalanceSubscriptionRequest, std::vector<BalanceEvent>>::Command;

  static const std::string name() { return "BalanceSubscription"; }

  static std::shared_ptr<ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new BalanceSubscription(std::make_shared<common::ClientSession>()));
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
      const BalanceSubscriptionRequest* request,
      std::vector<BalanceEvent>* events) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;
};
}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_BALANCE_SUBSCRIPTION_H_
