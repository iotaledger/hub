/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_SERVICE_USER_ADDRESS_MONITOR_H_
#define HUB_SERVICE_USER_ADDRESS_MONITOR_H_

#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "cppclient/api.h"
#include "hub/service/address_monitor.h"

namespace hub {
namespace service {

/// UserAddressMonitor class.
/// The user address monitor service is responsible for monitoring changes in
/// user addresses in the local database.
class UserAddressMonitor : public AddressMonitor {
 public:
  using AddressMonitor::AddressMonitor;

  /// constructor
  /// @param[in] api - an cppclient::IotaAPI compliant API provider
  /// @param[in] interval - the tick interval, in milliseconds,
  /// @param[in] fetchTransactionMessage - Should service fetch transaction's
  /// message
  explicit UserAddressMonitor(std::shared_ptr<cppclient::IotaAPI> api,
                              std::chrono::milliseconds interval,
                              bool fetchTransactionMessage)
      : AddressMonitor(std::move(api), interval),
        _fetchTransactionMessage(fetchTransactionMessage) {}

  std::vector<std::tuple<uint64_t, std::string>> monitoredAddresses() override;
  /// Process addresses that have changes
  /// @param[in] changed - a list of AddressMonitor::BalanceChange structures
  /// @return bool - true if successful
  bool onBalancesChanged(
      const std::vector<AddressMonitor::BalanceChange>& changed) override;

  std::unordered_map<uint64_t, uint64_t> initialBalances(
      std::vector<uint64_t> ids) override;

  /// @return string - the descriptive name of the service
  const std::string name() const override { return "UserAddressMonitor"; }

 protected:
  /// Check that the balance from the tangle is not less than the
  /// balance held in the database for the local address
  /// @param[in] address - the tangle address for which to check the balance
  /// @param[in] addressId - the database id of the local address
  /// @return bool - true if balance valid for this address
  bool validateBalanceIsConsistent(const std::string& address,
                                   uint64_t addressId);

 private:
  bool _fetchTransactionMessage;
};

}  // namespace service

}  // namespace hub

#endif  // HUB_SERVICE_USER_ADDRESS_MONITOR_H_
