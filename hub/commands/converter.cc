/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/converter.h"

namespace hub {
namespace cmd {

UserAccountBalanceEventType userAccountBalanceEventTypeFromSql(
    db::UserAccountBalanceReason reason) {
  using REASON = db::UserAccountBalanceReason;

  switch (reason) {
    case REASON::SWEEP:
      return cmd::UserAccountBalanceEventType::DEPOSIT;
    case REASON::BUY:
      return cmd::UserAccountBalanceEventType::BUY;
    case REASON::WITHDRAWAL:
      return cmd::UserAccountBalanceEventType::WITHDRAWAL;
    case REASON::WITHDRAWAL_CANCEL:
      return cmd::UserAccountBalanceEventType::WITHDRAWAL_CANCELED;
    case REASON::SELL:
      return cmd::UserAccountBalanceEventType::SELL;
    default:
      return cmd::UserAccountBalanceEventType::UAB_UNKNOWN;
  }
}

hub::rpc::UserAccountBalanceEventType userAccountBalanceEventTypeToProto(
    hub::cmd::UserAccountBalanceEventType type) {
  using TYPE = hub::cmd::UserAccountBalanceEventType;

  switch (type) {
    case TYPE::DEPOSIT:
      return rpc::UserAccountBalanceEventType::DEPOSIT;
    case TYPE::BUY:
      return rpc::UserAccountBalanceEventType::BUY;
    case TYPE::WITHDRAWAL:
      return rpc::UserAccountBalanceEventType::WITHDRAWAL;
    case TYPE::WITHDRAWAL_CANCELED:
      return rpc::UserAccountBalanceEventType::WITHDRAWAL_CANCELED;
    case TYPE::SELL:
      return rpc::UserAccountBalanceEventType::SELL;
    default:
      return rpc::UserAccountBalanceEventType::UAB_UNKNOWN;
  }
}

std::string userAccountBalanceEventTypeToString(
    hub::cmd::UserAccountBalanceEventType type) {
  using TYPE = hub::cmd::UserAccountBalanceEventType;

  switch (type) {
    case TYPE::DEPOSIT:
      return "DEPOSIT";
    case TYPE::BUY:
      return "BUY";
    case TYPE::WITHDRAWAL:
      return "WITHDRAWAL";
    case TYPE::WITHDRAWAL_CANCELED:
      return "WITHDRAWAL_CANCELED";
    case TYPE::SELL:
      return "SELL";
    default:
      return "UAB_UNKNOWN";
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
      return rpc::UserAddressBalanceReason::UADD_UNKNOWN;
  }
}

hub::rpc::UserAddressBalanceReason userAddressBalanceEventTypeToProto(
    hub::cmd::UserAccountBalanceEventType type) {
  using TYPE = hub::cmd::UserAddressBalanceEventType;

  switch (type) {
    case TYPE::UA_DEPOSIT:
      return rpc::UserAddressBalanceReason::UA_DEPOSIT;
    case TYPE::UA_SWEEP:
      return rpc::UserAddressBalanceReason::UA_SWEEP;

    default:
      return rpc::UserAddressBalanceReason::UADD_UNKNOWN;
  }
}

std::string userAddressBalanceEventTypeToString(
    hub::cmd::UserAccountBalanceEventType type) {
  using TYPE = hub::cmd::UserAddressBalanceEventType;

  switch (type) {
    case TYPE::UA_SWEEP:
      return "UA_SWEEP";
    case TYPE::UA_DEPOSIT:
      return "UA_DEPOSIT";
    default:
      return "UADD_UNKNOWN";
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
      return rpc::HubAddressBalanceReason::HUB_UNKNOWN;
  }
}

hub::rpc::HubAddressBalanceReason hubAddressBalanceTypeToProto(
    hub::cmd::HubAddressBalanceEventType type) {
  using TYPE = hub::cmd::HubAddressBalanceEventType;

  switch (type) {
    case TYPE::INBOUND:
      return rpc::HubAddressBalanceReason ::INBOUND;
    case TYPE::OUTBOUND:
      return rpc::HubAddressBalanceReason::OUTBOUND;

    default:
      return rpc::HubAddressBalanceReason::HUB_UNKNOWN;
  }
}

std::string hubAddressBalanceTypeToString(
    hub::cmd::HubAddressBalanceEventType type) {
  using TYPE = hub::cmd::HubAddressBalanceEventType;

  switch (type) {
    case TYPE::INBOUND:
      return "INBOUND";
    case TYPE::OUTBOUND:
      return "OUTBOUND";

    default:
      return "HUB_UNKNOWN";
  }
}

}  // namespace cmd

}  // namespace hub
