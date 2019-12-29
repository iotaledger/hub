/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_SERVICE_SCHEDULED_SERVICE_H_
#define HUB_SERVICE_SCHEDULED_SERVICE_H_

#include <chrono>
#include <memory>

#include <boost/asio/basic_waitable_timer.hpp>

#include "hub/service/service.h"

namespace hub {
namespace service {

/// ScheduledService abstract class. This is the base class for all scheduled
/// services. Each concrete subclass is responsible for implementing their
/// respective behaviour in the protected onTick() method
class ScheduledService : public Service {
 public:
  ScheduledService() = delete;

  /// constructor
  /// @param[in] interval - the tick interval in milliseconds
  explicit ScheduledService(std::chrono::milliseconds interval)
      : _interval(interval) {}

  /// Start the service
  void start() override;
  /// Stop the service
  void stop() override;
  /// Invoked at each tick occuring at <B>interval</B> milliseconds
  void tick();

 protected:
  /// Called by tick() by default
  /// @return false if it wants to stop.
  virtual bool doTick() = 0;

 protected:
  using Timer = boost::asio::basic_waitable_timer<std::chrono::steady_clock>;

  const std::chrono::milliseconds _interval;
  std::unique_ptr<Timer> _timer;
};

}  // namespace service

}  // namespace hub

#endif  // HUB_SERVICE_SCHEDULED_SERVICE_H_
