/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_BALANCE_EVENTS_H_
#define HUB_COMMANDS_BALANCE_EVENTS_H_

#include <string>

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
} UserAccountBalanceEventType;

typedef struct UserAccountBalanceEvent {
  std::string userId;
  uint64_t timestamp;
  UserAccountBalanceEventType type;
  int64_t amount;
  std::string sweepBundleHash;
  std::string withdrawalUUID;
} UserAccountBalanceEvent;

}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_BALANCE_EVENTS_H_
