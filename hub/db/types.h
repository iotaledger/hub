// Copyright 2018 IOTA Foundation

#ifndef HUB_DB_TYPES_H_
#define HUB_DB_TYPES_H_

#include <chrono>
#include <string>
#include <tuple>
#include <vector>

namespace hub {
namespace db {
enum class UserAddressBalanceReason { DEPOSIT = 0, SWEEP = 1 };

enum class HubAddressBalanceReason { INBOUND = 1, OUTBOUND = 2 };

enum class UserAccountBalanceReason {
  SWEEP = 0,
  BUY = 1,
  WITHDRAWAL_CANCEL = 2,
  WITHDRAWAL = 3,
  SELL = 4
};

using AddressWithID = std::tuple<uint64_t, std::string>;
using AddressWithUUID = std::tuple<std::string, std::string>;

struct UserBalanceEvent {
  std::string userIdentifier;
  std::chrono::system_clock::time_point timestamp;
  int64_t amount;
  UserAccountBalanceReason type;
};

struct Sweep {
  uint64_t id;
  std::string bundleHash;
  std::vector<std::string> trytes;
  uint64_t intoHubAddress;

  inline bool operator==(const Sweep& other) const {
    return id == other.id && bundleHash == other.bundleHash &&
           trytes == other.trytes && intoHubAddress == other.intoHubAddress;
  }
};

struct UserTransfer{
    uint64_t userId;
    int64_t  amount;
};

}  // namespace db
}  // namespace hub

#endif  // HUB_DB_TYPES_H_
