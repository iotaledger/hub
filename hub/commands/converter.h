/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_PROTO_SQL_CONVERTER_H_
#define HUB_COMMANDS_PROTO_SQL_CONVERTER_H_

#include <string>
#include "hub/db/types.h"
#include "proto/hub.pb.h"

#include "events.h"

namespace hub {

namespace cmd {

/// Account balance event
hub::cmd::UserAccountBalanceEventType userAccountBalanceEventTypeFromSql(
    hub::db::UserAccountBalanceReason reason);

hub::rpc::UserAccountBalanceEventType userAccountBalanceEventTypeToProto(
    hub::cmd::UserAccountBalanceEventType type);

std::string userAccountBalanceEventTypeToString(
    hub::cmd::UserAccountBalanceEventType type);

/// User address balance event
hub::cmd::UserAddressBalanceEventType userAddressBalanceEventTypeFromSql(
    hub::db::UserAddressBalanceReason reason);

hub::rpc::UserAddressBalanceReason userAddressBalanceEventTypeToProto(
    hub::cmd::UserAddressBalanceEventType type);

std::string userAddressBalanceEventTypeToString(
    hub::cmd::UserAddressBalanceEventType type);

/// Hub address balance event
hub::cmd::HubAddressBalanceEventType hubAddressBalanceTypeFromSql(
    hub::db::HubAddressBalanceReason reason);

hub::rpc::HubAddressBalanceReason hubAddressBalanceTypeToProto(
    hub::cmd::HubAddressBalanceEventType type);

std::string hubAddressBalanceTypeToString(
    hub::cmd::HubAddressBalanceEventType type);

}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_PROTO_SQL_CONVERTER_H_
