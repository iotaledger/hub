// Copyright 2018 IOTA Foundation

#include "hub/service/address_monitor.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <utility>

#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>

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

  // TODO(th0br0) Remove unmonitored addresses from internal list.
  // TODO(th0br0) Figure out better failure pattern.
  //              At the moment, on failure, getBalances will return an empty
  //              list. Therefore, nothing will happen.
  auto balances = _api->getBalances(std::move(addresses));

  for (auto& pair : balances) {
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
  persistBalanceChanges(calculateBalanceChanges());
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

}  // namespace service
}  // namespace hub
