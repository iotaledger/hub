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

#include "event_structs.h"

namespace hub {

namespace cmd {
hub::cmd::UserAccountBalanceEventType userAccountBalanceEventTypeFromSql(
    hub::db::UserAccountBalanceReason reason);
hub::rpc::UserAccountBalanceEventType userAccountBalanceEventTypeToProto(
    hub::cmd::UserAccountBalanceEventType type);
std::string userAccountBalanceEventTypeToString(
    hub::cmd::UserAccountBalanceEventType type);

hub::rpc::UserAddressBalanceReason userAddressBalanceEventTypeFromSql(
    hub::db::UserAddressBalanceReason reason);
hub::rpc::HubAddressBalanceReason hubAddressBalanceTypeFromSql(
    hub::db::HubAddressBalanceReason reason);

}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_PROTO_SQL_CONVERTER_H_
