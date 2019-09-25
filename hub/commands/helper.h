/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_HELPER_H_
#define HUB_COMMANDS_HELPER_H_

#include <string>

#include "proto/hub.pb.h"

namespace hub {

namespace cmd {
std::string errorToString(const hub::rpc::ErrorCode& e);

bool isAddressValid(std::string_view sv);
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_HELPER_H_
