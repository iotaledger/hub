#ifndef __HUB_SERVICE_ADDRESS_MONITOR_H_
#define __HUB_SERVICE_ADDRESS_MONITOR_H_

#include <chrono>
#include <cstdint>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

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
  virtual std::unordered_set<std::tuple<uint64_t, std::string>>
  monitoredAddresses() = 0;
  virtual void onBalancesChanged(std::unordered_set<BalanceChange> changed) = 0;

  const std::string name() const override { return "AddressMonitor"; }

 private:
  std::unordered_map<std::string, uint64_t> _balances;
};

}  // namespace service

}  // namespace hub

#endif /*__HUB_SERVICE_ADDRESS_MONITOR_H_ */
