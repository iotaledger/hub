/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/proto_sql_converter.h"

namespace hub {
namespace cmd {

rpc::UserAccountBalanceEventType userAccountBalanceEventTypeFromSql(
    db::UserAccountBalanceReason reason) {
  using REASON = db::UserAccountBalanceReason;

  switch (reason) {
    case REASON::SWEEP:
      return rpc::UserAccountBalanceEventType::DEPOSIT;
    case REASON::BUY:
      return rpc::UserAccountBalanceEventType::BUY;
    case REASON::WITHDRAWAL:
      return rpc::UserAccountBalanceEventType::WITHDRAWAL;
    case REASON::WITHDRAWAL_CANCEL:
      return rpc::UserAccountBalanceEventType::WITHDRAWAL_CANCELED;
    case REASON::SELL:
      return rpc::UserAccountBalanceEventType::SELL;
    default:
      return rpc::UserAccountBalanceEventType::UE_UNKNOWN;
  }
}

rpc::UserAddressBalanceReason userAddressBalanceEventTypeFromSql(
    hub::db::UserAddressBalanceReason reason) {
  using REASON = db::UserAddressBalanceReason;

  switch (reason) {
    case REASON::DEPOSIT:
      return rpc::UserAddressBalanceReason::UA_DEPOSIT;
    case REASON::SWEEP:
      return rpc::UserAddressBalanceReason::UA_SWEEP;
    default:
      return rpc::UserAddressBalanceReason::UAB_UNKNOWN;
  }
}

hub::rpc::HubAddressBalanceReason hubAddressBalanceTypeFromSql(
    hub::db::HubAddressBalanceReason reason) {
  using REASON = db::HubAddressBalanceReason;

  switch (reason) {
    case REASON::INBOUND:
      return rpc::HubAddressBalanceReason::INBOUND;
    case REASON::OUTBOUND:
      return rpc::HubAddressBalanceReason::OUTBOUND;
    default:
      return rpc::HubAddressBalanceReason::HAB_UNKNOWN;
  }
}

}  // namespace cmd

}  // namespace hub
