#ifndef __HUB_COMMANDS_HELPER_H_
#define __HUB_COMMANDS_HELPER_H_

#include "proto/hub.pb.h"

namespace iota {

namespace cmd {
std::string errorToString(const iota::rpc::ErrorCode& e);
}
}  // namespace iota

#endif /* __HUB_COMMANDS_HELPER_H_ */
