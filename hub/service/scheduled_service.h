// Copyright 2018 IOTA Foundation

#ifndef HUB_SERVICE_SCHEDULED_SERVICE_H_
#define HUB_SERVICE_SCHEDULED_SERVICE_H_

#include <chrono>
#include <memory>

#include <boost/asio/basic_waitable_timer.hpp>

#include "hub/service/service.h"

namespace hub {
namespace service {

class ScheduledService : public Service {
 public:
  ScheduledService() = delete;

  explicit ScheduledService(std::chrono::milliseconds interval)
      : _interval(interval) {}

  void start() override;
  void stop() override;
  void tick();

 protected:
  //! Should return false if it wants to stop.
  virtual bool doTick() = 0;

 protected:
  using Timer = boost::asio::basic_waitable_timer<std::chrono::steady_clock>;

  const std::chrono::milliseconds _interval;
  std::unique_ptr<Timer> _timer;
};

}  // namespace service

}  // namespace hub

#endif  // HUB_SERVICE_SCHEDULED_SERVICE_H_
