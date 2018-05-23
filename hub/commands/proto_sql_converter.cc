/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */


#include "hub/commands/proto_sql_converter.h"

namespace hub {
namespace cmd {

rpc::UserBalanceEventType userBalanceEventTypeFromSql(
    db::UserAccountBalanceReason reason) {
  using REASON = db::UserAccountBalanceReason;

  switch (reason) {
    case REASON::SWEEP:
      return rpc::UserBalanceEventType::DEPOSIT;
    case REASON::BUY:
      return rpc::UserBalanceEventType::BUY;
    case REASON::WITHDRAWAL:
      return rpc::UserBalanceEventType::WITHDRAWAL;
    case REASON::WITHDRAWAL_CANCEL:
      return rpc::UserBalanceEventType::WITHDRAWAL_CANCELED;
    case REASON::SELL:
      return rpc::UserBalanceEventType::SELL;
    default:
      return rpc::UserBalanceEventType::UE_UNKNOWN;
  }
}
}  // namespace cmd

}  // namespace hub
