/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/converter.h"

namespace hub {
namespace cmd {

UserAccountBalanceEventReason userAccountBalanceEventReasonFromSql(
    db::UserAccountBalanceReason reason) {
  using REASON = db::UserAccountBalanceReason;

  switch (reason) {
    case REASON::SWEEP:
      return cmd::UserAccountBalanceEventReason::DEPOSIT;
    case REASON::BUY:
      return cmd::UserAccountBalanceEventReason::BUY;
    case REASON::WITHDRAWAL:
      return cmd::UserAccountBalanceEventReason::WITHDRAWAL;
    case REASON::WITHDRAWAL_CANCEL:
      return cmd::UserAccountBalanceEventReason::WITHDRAWAL_CANCELED;
    case REASON::SELL:
      return cmd::UserAccountBalanceEventReason::SELL;
    default:
      return cmd::UserAccountBalanceEventReason::UAB_UNKNOWN;
  }
}

hub::rpc::UserAccountBalanceEventType userAccountBalanceEventReasonToProto(
    UserAccountBalanceEventReason reason) {
  using REASON = hub::cmd::UserAccountBalanceEventReason;

  switch (reason) {
    case REASON::DEPOSIT:
      return rpc::UserAccountBalanceEventType::DEPOSIT;
    case REASON::BUY:
      return rpc::UserAccountBalanceEventType::BUY;
    case REASON::WITHDRAWAL:
      return rpc::UserAccountBalanceEventType::WITHDRAWAL;
    case REASON::WITHDRAWAL_CANCELED:
      return rpc::UserAccountBalanceEventType::WITHDRAWAL_CANCELED;
    case REASON::SELL:
      return rpc::UserAccountBalanceEventType::SELL;
    default:
      return rpc::UserAccountBalanceEventType::UAB_UNKNOWN;
  }
}

std::string userAccountBalanceEventReasonToString(
    UserAccountBalanceEventReason reason) {
  using REASON = hub::cmd::UserAccountBalanceEventReason;

  switch (reason) {
    case REASON::DEPOSIT:
      return "DEPOSIT";
    case REASON::BUY:
      return "BUY";
    case REASON::WITHDRAWAL:
      return "WITHDRAWAL";
    case REASON::WITHDRAWAL_CANCELED:
      return "WITHDRAWAL_CANCELED";
    case REASON::SELL:
      return "SELL";
    default:
      return "UAB_UNKNOWN";
  }
}

hub::cmd::UserAddressBalanceEventReason userAddressBalanceEventReasonFromSql(
    hub::db::UserAddressBalanceReason reason) {
  using REASON = db::UserAddressBalanceReason;

  switch (reason) {
    case REASON::DEPOSIT:
      return hub::cmd::UserAddressBalanceEventReason::UA_DEPOSIT;
    case REASON::SWEEP:
      return hub::cmd::UserAddressBalanceEventReason::UA_SWEEP;
    default:
      return hub::cmd::UserAddressBalanceEventReason::UADD_UNKNOWN;
  }
}

hub::rpc::UserAddressBalanceReason userAddressBalanceEventReasonToProto(
    UserAddressBalanceEventReason reason) {
  using REASON = hub::cmd::UserAddressBalanceEventReason;

  switch (reason) {
    case REASON::UA_DEPOSIT:
      return rpc::UserAddressBalanceReason::UA_DEPOSIT;
    case REASON::UA_SWEEP:
      return rpc::UserAddressBalanceReason::UA_SWEEP;

    default:
      return rpc::UserAddressBalanceReason::UADD_UNKNOWN;
  }
}

std::string userAddressBalanceEventReasonToString(
    UserAddressBalanceEventReason reason) {
  using REASON = hub::cmd::UserAddressBalanceEventReason;

  switch (reason) {
    case REASON::UA_SWEEP:
      return "UA_SWEEP";
    case REASON::UA_DEPOSIT:
      return "UA_DEPOSIT";
    default:
      return "UADD_UNKNOWN";
  }
}

hub::cmd::HubAddressBalanceEventReason hubAddressBalanceReasonFromSql(
    hub::db::HubAddressBalanceReason reason) {
  using REASON = db::HubAddressBalanceReason;

  switch (reason) {
    case REASON::INBOUND:
      return hub::cmd::HubAddressBalanceEventReason::INBOUND;
    case REASON::OUTBOUND:
      return hub::cmd::HubAddressBalanceEventReason::OUTBOUND;
    default:
      return hub::cmd::HubAddressBalanceEventReason::HUB_UNKNOWN;
  }
}

hub::rpc::HubAddressBalanceReason hubAddressBalanceReasonToProto(
    HubAddressBalanceEventReason reason) {
  using REASON = hub::cmd::HubAddressBalanceEventReason;

  switch (reason) {
    case REASON::INBOUND:
      return rpc::HubAddressBalanceReason ::INBOUND;
    case REASON::OUTBOUND:
      return rpc::HubAddressBalanceReason::OUTBOUND;

    default:
      return rpc::HubAddressBalanceReason::HUB_UNKNOWN;
  }
}

std::string hubAddressBalanceReasonToString(
    HubAddressBalanceEventReason reason) {
  using REASON = hub::cmd::HubAddressBalanceEventReason;

  switch (reason) {
    case REASON::INBOUND:
      return "INBOUND";
    case REASON::OUTBOUND:
      return "OUTBOUND";

    default:
      return "HUB_UNKNOWN";
  }
}

}  // namespace cmd

}  // namespace hub
