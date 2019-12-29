// Copyright 2018 IOTA Foundation

#ifndef HUB_COMMANDS_SWEEP_SUBSCRIPTION_H_
#define HUB_COMMANDS_SWEEP_SUBSCRIPTION_H_

#include <string>
#include <string_view>
#include <vector>

#include "common/commands/command.h"
#include "events.h"

namespace hub {

namespace cmd {

typedef struct SweepSubscriptionRequest {
  uint64_t newerThan;
} SweepSubscriptionRequest;

class SweepSubscription : public common::Command<SweepSubscriptionRequest,
                                                 std::vector<SweepEvent>> {
 public:
  using Command<SweepSubscriptionRequest, std::vector<SweepEvent>>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new SweepSubscription(std::make_shared<common::ClientSession>()));
  }

  common::cmd::Error doProcess(
      const SweepSubscriptionRequest* request,
      std::vector<SweepEvent>* events) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;

  static const std::string name() { return "SweepSubscription"; }

  virtual std::vector<db::SweepEvent> getSweeps(
      std::chrono::system_clock::time_point lastCheck);
};
}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_SWEEP_SUBSCRIPTION_H_
