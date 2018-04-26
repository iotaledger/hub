// Copyright 2018 IOTA Foundation

#ifndef HUB_COMMANDS_HELPER_H_
#define HUB_COMMANDS_HELPER_H_

#include <string>

#include "proto/hub.pb.h"

namespace hub {

namespace cmd {
std::string errorToString(const hub::rpc::ErrorCode& e);
}
}  // namespace hub

#endif  // HUB_COMMANDS_HELPER_H_
