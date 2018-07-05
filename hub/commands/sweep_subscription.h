// Copyright 2018 IOTA Foundation

#ifndef HUB_COMMANDS_SWEEP_SUBSCRIPTION_H_
#define HUB_COMMANDS_SWEEP_SUBSCRIPTION_H_

#include <string>
#include <vector>

#include <grpc++/support/sync_stream.h>

#include "common/command.h"
#include "hub/db/helper.h"

namespace hub {
namespace rpc {
class SweepSubscriptionRequest;
class SweepEvent;
}  // namespace rpc

namespace cmd {

class SweepSubscription
    : public common::Command<
          hub::rpc::SweepSubscriptionRequest,
          grpc::ServerWriterInterface<hub::rpc::SweepEvent>> {
 public:
  using Command<hub::rpc::SweepSubscriptionRequest,
                grpc::ServerWriterInterface<hub::rpc::SweepEvent>>::Command;

  grpc::Status doProcess(const hub::rpc::SweepSubscriptionRequest* request,
                         grpc::ServerWriterInterface<hub::rpc::SweepEvent>*
                             writer) noexcept override;

  const std::string name() override { return "SweepSubscription"; }

  virtual std::vector<db::SweepEvent> getSweeps(
      std::chrono::system_clock::time_point lastCheck);
};
}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_SWEEP_SUBSCRIPTION_H_
