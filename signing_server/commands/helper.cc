/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include <sstream>
#include <string>

#include "proto/signing_server_messages.pb.h"
#include "signing_server/commands/helper.h"

namespace signing {

namespace cmd {
std::string errorToString(const signing::rpc::ErrorCode& e) {
  std::ostringstream ss;
  signing::rpc::Error error;

  error.set_code(e);
  error.SerializeToOstream(&ss);

  return ss.str();
}
}  // namespace cmd
}  // namespace signing
