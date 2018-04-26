// Copyright 2018 IOTA Foundation

#include "hub/commands/helper.h"

#include <sstream>
#include <string>

#include "proto/hub.pb.h"

namespace hub {
namespace cmd {
std::string errorToString(const hub::rpc::ErrorCode& e) {
  std::ostringstream ss;
  hub::rpc::Error error;

  error.set_code(e);
  error.SerializeToOstream(&ss);

  return ss.str();
}
}  // namespace cmd
}  // namespace hub
