#include "address_monitor.h"

#include <iterator>
#include <stdexcept>
#include <tuple>
#include <unordered_map>

#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>

namespace hub {
namespace service {

std::vector<AddressMonitor::BalanceChange> AddressMonitor::updateBalances() {
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

  auto balances = _api->getBalances(std::move(addresses));

  for (auto& pair : balances) {
    auto id = addressToIDs[pair.first];

    // This will create current (= 0) if the element does not exist.
    int64_t prev = _balances[id];
    _balances[id] = pair.second;

    int64_t delta = ((int64_t)pair.second) - prev;

    if (delta) {
      changes.push_back({id, std::move(pair.first), pair.second, delta});
    }
  }

  return changes;
}

void AddressMonitor::onStart() { updateBalances(); }

bool AddressMonitor::doTick() {
  auto changes = updateBalances();

  if (!changes.empty()) {
    onBalancesChanged(std::move(changes));
  }

  return true;
}

}  // namespace service
}  // namespace hub
