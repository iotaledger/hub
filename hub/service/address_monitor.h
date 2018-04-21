#ifndef __HUB_SERVICE_ADDRESS_MONITOR_H_
#define __HUB_SERVICE_ADDRESS_MONITOR_H_

#include <chrono>
#include <string>

#include "scheduled_service.h"

namespace hub {
namespace service {

class AddressMonitor : public ScheduledService {
 public:
  explicit AddressMonitor(std::chrono::milliseconds interval)
      : ScheduledService(interval) {}

  bool doTick() override;

  const std::string name() const override { return "AddressMonitor"; }
};

}  // namespace service

}  // namespace hub

#endif /*__HUB_SERVICE_ADDRESS_MONITOR_H_ */
