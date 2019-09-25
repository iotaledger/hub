/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
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
hub::cmd::UserAccountBalanceEventReason userAccountBalanceEventReasonFromSql(
    hub::db::UserAccountBalanceReason reason);

hub::rpc::UserAccountBalanceEventType userAccountBalanceEventReasonToProto(
    hub::cmd::UserAccountBalanceEventReason reason);

std::string userAccountBalanceEventReasonToString(
    hub::cmd::UserAccountBalanceEventReason reason);

/// User address balance event
hub::cmd::UserAddressBalanceEventReason userAddressBalanceEventReasonFromSql(
    hub::db::UserAddressBalanceReason reason);

hub::rpc::UserAddressBalanceReason userAddressBalanceEventReasonToProto(
    hub::cmd::UserAddressBalanceEventReason reason);

std::string userAddressBalanceEventReasonToString(
    hub::cmd::UserAddressBalanceEventReason reason);

/// Hub address balance event
hub::cmd::HubAddressBalanceEventReason hubAddressBalanceReasonFromSql(
    hub::db::HubAddressBalanceReason reason);

hub::rpc::HubAddressBalanceReason hubAddressBalanceReasonToProto(
    hub::cmd::HubAddressBalanceEventReason reason);

std::string hubAddressBalanceReasonToString(
    hub::cmd::HubAddressBalanceEventReason reason);

}  // namespace cmd
}  // namespace hub
#endif  // HUB_COMMANDS_PROTO_SQL_CONVERTER_H_
