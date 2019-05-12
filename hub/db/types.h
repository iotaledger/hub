/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_DB_TYPES_H_
#define HUB_DB_TYPES_H_

#include <chrono>
#include <nonstd/optional.hpp>
#include <string>
#include <tuple>
#include <vector>

#include "common/crypto/types.h"

namespace hub {
namespace db {
enum class UserAddressBalanceReason { DEPOSIT = 0, SWEEP = 1 };

enum class HubAddressBalanceReason { INBOUND = 0, OUTBOUND = 1 };

enum class UserAccountBalanceReason {
  SWEEP = 0,
  BUY = 1,
  WITHDRAWAL_CANCEL = 2,
  WITHDRAWAL = 3,
  SELL = 4
};

using AddressWithID = std::tuple<uint64_t, std::string>;
using AddressWithUUID = std::tuple<std::string, std::string>;

struct UserAccountBalanceEvent {
  std::string userIdentifier;
  std::chrono::system_clock::time_point timestamp;
  int64_t amount;
  UserAccountBalanceReason type;
  std::string sweepBundleHash;
  std::string withdrawalUUID;
};

struct UserAddressBalanceEvent {
  std::string userIdentifier;
  std::string userAddress;
  int64_t amount;
  UserAddressBalanceReason reason;
  // Depositing bundle's tail (if reason == DEPOSIT)
  // Bundle hash of sweep (if reason == SWEEP)
  std::string hash;
  std::chrono::system_clock::time_point timestamp;
  std::string message;
};

struct HubAddressBalanceEvent {
  std::string hubAddress;
  int64_t amount;
  HubAddressBalanceReason reason;
  std::string sweepBundleHash;
  std::chrono::system_clock::time_point timestamp;
};

struct SweepEvent {
  std::string bundleHash;
  std::chrono::system_clock::time_point timestamp;
  std::vector<std::string> withdrawalUUIDs;
};

struct SweepDetail {
  bool confirmed;
  std::vector<std::string> trytes;
  std::vector<common::crypto::Hash> tails;
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

struct UserTransfer {
  uint64_t userId;
  int64_t amount;
};

struct WithdrawalInfo {
  uint64_t id;
  uint64_t userId;
  int64_t amount;
};

struct TransferInput {
 public:
  int64_t addressId;
  int64_t userId;
  common::crypto::Address address;
  common::crypto::UUID uuid;
  uint64_t amount;
};

struct TransferOutput {
 public:
  int64_t id;
  uint64_t amount;
  nonstd::optional<common::crypto::Tag> tag;
  common::crypto::Address payoutAddress;
};

struct AddressInfo {
 public:
  std::string userId;
  common::crypto::UUID uuid;
  bool usedForSweep;
};

struct SweepTail {
  std::string hash;
  std::chrono::system_clock::time_point createdAt;
};

}  // namespace db
}  // namespace hub

#endif  // HUB_DB_TYPES_H_
