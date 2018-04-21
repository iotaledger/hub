#ifndef __HUB_COMMANDS_HELPER_H_
#define __HUB_COMMANDS_HELPER_H_

#include "proto/hub.pb.h"

namespace hub {

namespace cmd {
std::string errorToString(const hub::rpc::ErrorCode& e);
}
}  // namespace hub

#endif /* __HUB_COMMANDS_HELPER_H_ */
