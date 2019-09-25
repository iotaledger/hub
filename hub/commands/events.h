/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_BALANCE_EVENTS_H_
#define HUB_COMMANDS_BALANCE_EVENTS_H_

#include <string>
#include <variant>

namespace hub {
namespace cmd {

typedef enum {
  // Unused
  UAB_UNKNOWN = 0,
  // Deposit into user account (positive amount)
  DEPOSIT,
  // User received tokens as part of a transfer batch (positive amount)
  BUY,
  // User withdrawal request (negative amount)
  WITHDRAWAL,
  // Cancelled user withdrawal request (positive amount)
  WITHDRAWAL_CANCELED,
  // User lost tokens as part of a transfer batch (negative amount)
  SELL
} UserAccountBalanceEventReason;

typedef enum {
  UADD_UNKNOWN = 0,
  // New user deposit tracked
  UA_DEPOSIT = 1,
  // Hub-sweep.
  UA_SWEEP = 2
} UserAddressBalanceEventReason;

typedef enum {
  HUB_UNKNOWN = 0,
  // Sweep inbound (used as reminader address)
  INBOUND = 1,
  // Sweep outbound (used as input)
  OUTBOUND = 2
} HubAddressBalanceEventReason;

typedef struct UserAccountBalanceEvent {
  std::string userId;
  uint64_t timestamp;
  UserAccountBalanceEventReason reason;
  int64_t amount;
  std::string sweepBundleHash;
  std::string withdrawalUUID;
} UserAccountBalanceEvent;

typedef struct UserAddressBalanceEvent {
  std::string userId;
  std::string userAddress;
  int64_t amount;
  UserAddressBalanceEventReason reason;
  std::string hash;
  uint64_t timestamp;
  std::string message;
} UserAddressBalanceEvent;

typedef struct HubAddressBalanceEvent {
  std::string hubAddress;
  int64_t amount;
  HubAddressBalanceEventReason reason;
  std::string sweepBundleHash;
  uint64_t timestamp;
} HubAddressBalanceEvent;

class BalanceEvent {
 public:
  BalanceEvent(const UserAccountBalanceEvent&& e) {
    _variant.emplace<UserAccountBalanceEvent>(e);
  }

  BalanceEvent(const UserAddressBalanceEvent&& e) {
    _variant.emplace<UserAddressBalanceEvent>(e);
  }

  BalanceEvent(const HubAddressBalanceEvent&& e) {
    _variant.emplace<HubAddressBalanceEvent>(e);
  }

  std::variant<UserAccountBalanceEvent, UserAddressBalanceEvent,
               HubAddressBalanceEvent>&
  getVariant() {
    return _variant;
  }

 private:
  std::variant<UserAccountBalanceEvent, UserAddressBalanceEvent,
               HubAddressBalanceEvent>
      _variant;
};

typedef struct SweepEvent {
  std::string bundleHash;
  uint64_t timestamp;
  std::vector<std::string> uuids;
} SweepEvent;

}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_BALANCE_EVENTS_H_
