// Copyright 2018 IOTA Foundation

#ifndef HUB_SERVICE_ADDRESS_MONITOR_H_
#define HUB_SERVICE_ADDRESS_MONITOR_H_

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "hub/iota/api.h"
#include "hub/service/scheduled_service.h"

namespace hub {
namespace service {

class AddressMonitor : public ScheduledService {
 public:
  struct BalanceChange {
   public:
    const uint64_t addressId;
    const std::string address;
    const uint64_t balance;
    const int64_t delta;

   private:
    inline auto tie() const {
      return std::tie(addressId, address, balance, delta);
    }

   public:
    inline bool operator==(const BalanceChange& rhs) const {
      return tie() == rhs.tie();
    }
  };

  explicit AddressMonitor(std::shared_ptr<hub::iota::IotaAPI> api,
                          std::chrono::milliseconds interval)
      : ScheduledService(interval), _api(std::move(api)) {}
  virtual ~AddressMonitor() {}

  virtual std::vector<std::tuple<uint64_t, std::string>>
  monitoredAddresses() = 0;

  //! @return true if balance changes are accepted
  virtual bool onBalancesChanged(const std::vector<BalanceChange>& changed) = 0;

  virtual std::unordered_map<uint64_t, uint64_t> initialBalances(
      std::vector<uint64_t> ids) = 0;

  void onStart() override;

  // copy
  inline std::unordered_map<uint64_t, uint64_t> balances() const {
    return _balances;
  }

 protected:
  bool doTick() override;

  std::vector<BalanceChange> calculateBalanceChanges();

  void persistBalanceChanges(std::vector<BalanceChange> changes);
  void removeUnmonitoredAddresses(
      const std::unordered_map<std::string, uint64_t>& addressToIds);

 protected:
  const std::shared_ptr<hub::iota::IotaAPI> _api;

 private:
  std::unordered_map<uint64_t, uint64_t> _balances;
};

}  // namespace service
}  // namespace hub

#endif  // HUB_SERVICE_ADDRESS_MONITOR_H_
