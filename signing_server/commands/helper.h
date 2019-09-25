/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef SIGNING_SERVER_COMMANDS_HELPER_H_
#define SIGNING_SERVER_COMMANDS_HELPER_H_

#include <string>

#include "proto/signing_server.pb.h"

namespace signing {

namespace cmd {
std::string errorToString(const signing::rpc::ErrorCode& e);
}
}  // namespace signing

#endif  // SIGNING_SERVER_COMMANDS_HELPER_H_
