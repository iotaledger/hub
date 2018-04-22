#ifndef __HUB_SERVICE_ADDRESS_MONITOR_H_
#define __HUB_SERVICE_ADDRESS_MONITOR_H_

#include <chrono>
#include <cstdint>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "scheduled_service.h"

namespace hub {
namespace service {

class AddressMonitor : public ScheduledService {
 public:
  struct BalanceChange {
    uint64_t id;
    std::string address;
    int64_t delta;
  };

  explicit AddressMonitor(std::chrono::milliseconds interval)
      : ScheduledService(interval) {}

  bool doTick() override;

  virtual std::vector<std::tuple<uint64_t, std::string>>
  monitoredAddresses() = 0;

  virtual void onBalancesChanged(std::vector<BalanceChange> changed) = 0;

 private:
  std::unordered_map<uint64_t, uint64_t> _balances;
};

}  // namespace service

}  // namespace hub

#endif /*__HUB_SERVICE_ADDRESS_MONITOR_H_ */
