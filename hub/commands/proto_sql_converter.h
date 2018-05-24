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

namespace hub {

namespace cmd {
hub::rpc::UserBalanceEventType userBalanceEventTypeFromSql(
    hub::db::UserAccountBalanceReason reason);
}

}  // namespace hub
#endif  // HUB_COMMANDS_PROTO_SQL_CONVERTER_H_
