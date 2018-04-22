#ifndef __HUB_SERVICE_SCHEDULED_SERVICE_H_
#define __HUB_SERVICE_SCHEDULED_SERVICE_H_

#include <chrono>
#include <memory>

#include <boost/asio/basic_waitable_timer.hpp>

#include "service.h"

namespace hub {
namespace service {

class ScheduledService : public Service {
 public:
  ScheduledService() = delete;
  
  explicit ScheduledService(std::chrono::milliseconds interval)
      : _interval(interval) {}

  virtual void start() override;
  virtual void stop() override;
  void tick();

  //! Should return false if it wants to stop.
  virtual bool  doTick() = 0;

 protected:
  using Timer = boost::asio::basic_waitable_timer<std::chrono::steady_clock>;

  const std::chrono::milliseconds _interval;
  std::unique_ptr<Timer> _timer;
};

}  // namespace service

}  // namespace hub

#endif /*__HUB_SERVICE_SCHEDULED_SERVICE_H_ */
