/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

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

#include "cppclient/api.h"
#include "hub/service/scheduled_service.h"

namespace hub {
namespace service {

/// Class AddressMonitor. Defines the abstract behaviour of a service that
/// monitors changes of addresses.
class AddressMonitor : public ScheduledService {
 public:
  /// Structure BalanceChange. Contains the changes found at that address
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

  /// constructor
  /// @param[in] api - an cppclient::IotaAPI compliant API provider
  /// @param[in] interval - the tick interval, in milliseconds,
  explicit AddressMonitor(std::shared_ptr<cppclient::IotaAPI> api,
                          std::chrono::milliseconds interval)
      : ScheduledService(interval), _api(std::move(api)) {}
  /// Destructor
  virtual ~AddressMonitor() {}

  /// The list of addresses that are monitored
  virtual std::vector<std::tuple<uint64_t, std::string>>
  monitoredAddresses() = 0;

  /// @param[in] changed - a list of addresses that have changed
  //! @return true if balance changes are accepted
  virtual bool onBalancesChanged(const std::vector<BalanceChange>& changed) = 0;

  virtual std::unordered_map<uint64_t, uint64_t> initialBalances(
      std::vector<uint64_t> ids) = 0;

  void onStart() override;

  /// @return the list of persisted balances
  inline std::unordered_map<uint64_t, uint64_t> balances() const {
    return _balances;
  }

 protected:
  /// Process each tick at the interval specified in the constructor
  bool doTick() override;

  /// Calculate balance changes
  /// @return a list of BalanceChange structures
  std::vector<BalanceChange> calculateBalanceChanges();

  void persistBalanceChanges(std::vector<BalanceChange> changes);
  void removeUnmonitoredAddresses(
      const std::unordered_map<std::string, uint64_t>& addressToIds);

 protected:
  /// an cppclient::IotaAPI compliant API provider
  const std::shared_ptr<cppclient::IotaAPI> _api;

 private:
  template <typename T>
  static std::vector<T> nextBatch(const std::vector<T>& vec,
                                  uint32_t& numBatchedEntries,
                                  uint32_t batchSize);
  /// A list of persisted balances ebtween calls
  std::unordered_map<uint64_t, uint64_t> _balances;
};

}  // namespace service
}  // namespace hub

#endif  // HUB_SERVICE_ADDRESS_MONITOR_H_
