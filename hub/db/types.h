// Copyright 2018 IOTA Foundation

#ifndef HUB_DB_TYPES_H_
#define HUB_DB_TYPES_H_

#include <chrono>
#include <string>
#include <tuple>

#include <boost/uuid/uuid.hpp>

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
using AddressWithUUID = std::tuple<std::string, boost::uuids::uuid>;

struct UserBalanceEvent {
  std::chrono::system_clock::time_point timestamp;
  int64_t amount;
  UserAccountBalanceReason type;
};

struct Sweep {
  uint64_t id;
  std::string bundleHash;
  std::string trytes;
  uint64_t intoHubAddress;

  inline bool operator==(const Sweep& other) const {
    return id == other.id && bundleHash == other.bundleHash &&
           trytes == other.bundleHash && intoHubAddress == other.intoHubAddress;
  }
};

}  // namespace db
}  // namespace hub

namespace std {
template <>
struct hash<hub::db::Sweep> {
  typedef hub::db::Sweep argument_type;
  typedef std::size_t result_type;
  result_type operator()(argument_type const& s) const noexcept {
    result_type const h1(std::hash<uint64_t>{}(s.id));
    result_type const h2(std::hash<std::string>{}(s.trytes));
    return h1 ^ (h2 << 1);
  }
};

}  // namespace std

#endif  // HUB_DB_TYPES_H_
