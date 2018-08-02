/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/service/address_monitor.h"

#include <algorithm>
#include <iterator>
#include <set>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <utility>

#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm/copy.hpp>

namespace hub {
namespace service {

std::vector<AddressMonitor::BalanceChange>
AddressMonitor::calculateBalanceChanges() {
  auto monitored = monitoredAddresses();

  std::unordered_map<std::string, uint64_t> addressToIDs;
  std::vector<std::string> addresses;
  std::vector<BalanceChange> changes;

  // monitored -> map<address, id>
  std::transform(
      std::begin(monitored), std::end(monitored),
      std::inserter(addressToIDs, addressToIDs.begin()), [](const auto& tup) {
        return std::pair{std::move(std::get<1>(tup)), std::get<0>(tup)};
      });

  // map<address, id> -> list<address>
  std::transform(std::begin(addressToIDs), std::end(addressToIDs),
                 std::back_inserter(addresses),
                 [](const auto& pair) { return pair.first; });

  removeUnmonitoredAddresses(addressToIDs);
  if (addresses.empty()) {
    return {};
  }
  // TODO(th0br0) Figure out better failure pattern.
  //              At the moment, on failure, getBalances will return an empty
  //              list. Therefore, nothing will happen.
  auto balances = _api->getBalances(std::move(addresses));

  if (!balances) {
    return {};
  }

  for (auto& pair : *balances) {
    auto id = addressToIDs[pair.first];

    // This will create current (= 0) if the element does not exist.
    int64_t prev = _balances[id];
    int64_t delta = ((int64_t)pair.second) - prev;

    if (delta) {
      changes.push_back({id, std::move(pair.first), pair.second, delta});
    }
  }

  return changes;
}

void AddressMonitor::onStart() {
  auto monitored = monitoredAddresses();

  std::vector<uint64_t> ids;

  for (const auto& tup : monitored) {
    ids.push_back(std::get<0>(tup));
  }

  _balances = initialBalances(std::move(ids));
}

void AddressMonitor::persistBalanceChanges(
    std::vector<AddressMonitor::BalanceChange> changes) {
  for (const auto& chg : changes) {
    _balances[chg.addressId] = chg.balance;
  }
}

bool AddressMonitor::doTick() {
  auto changes = calculateBalanceChanges();

  if (!changes.empty()) {
    if (onBalancesChanged(changes)) {
      persistBalanceChanges(std::move(changes));
    }
  }

  return true;
}

void AddressMonitor::removeUnmonitoredAddresses(
    const std::unordered_map<std::string, uint64_t>& addressToIds) {
  std::set<uint64_t> monitoredIds;
  boost::copy(addressToIds | boost::adaptors::map_values,
              std::inserter(monitoredIds, monitoredIds.begin()));

  std::set<uint64_t> balancesIds;
  boost::copy(_balances | boost::adaptors::map_keys,
              std::inserter(balancesIds, balancesIds.begin()));

  std::vector<uint64_t> unmonitoredBalances;
  std::set_difference(balancesIds.begin(), balancesIds.end(),
                      monitoredIds.begin(), monitoredIds.end(),
                      std::back_inserter(unmonitoredBalances));

  for (auto id : unmonitoredBalances) {
    _balances.erase(id);
  }
}

}  // namespace service
}  // namespace hub
