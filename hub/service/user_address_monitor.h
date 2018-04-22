#ifndef __HUB_SERVICE_USER_ADDRESS_MONITOR_H_
#define __HUB_SERVICE_USER_ADDRESS_MONITOR_H_

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

#include "address_monitor.h"

namespace hub {
namespace service {

class UserAddressMonitor : public AddressMonitor {
 public:
  using AddressMonitor::AddressMonitor;

  std::vector<std::tuple<uint64_t, std::string>> monitoredAddresses() override;
  void onBalancesChanged(
      std::vector<AddressMonitor::BalanceChange> changed) override {}

  const std::string name() const override { return "UserAddressMonitor"; }
};

}  // namespace service

}  // namespace hub

#endif /*__HUB_SERVICE_USER_ADDRESS_MONITOR_H_ */
