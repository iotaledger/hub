// Copyright 2018 IOTA Foundation

#ifndef HUB_SERVICE_USER_ADDRESS_MONITOR_H_
#define HUB_SERVICE_USER_ADDRESS_MONITOR_H_

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

#include "hub/iota/api.h"
#include "hub/service/address_monitor.h"

namespace hub {
namespace service {

class UserAddressMonitor : public AddressMonitor {
 public:
  using AddressMonitor::AddressMonitor;

  std::vector<std::tuple<uint64_t, std::string>> monitoredAddresses() override;
  bool onBalancesChanged(
      const std::vector<AddressMonitor::BalanceChange>& changed) override;

  const std::string name() const override { return "UserAddressMonitor"; }

 protected:
  bool validateBalanceIsConsistent(const std::string& address,
                                   uint64_t addressId);
};

}  // namespace service

}  // namespace hub

#endif  // HUB_SERVICE_USER_ADDRESS_MONITOR_H_
