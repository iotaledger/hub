/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMON_GRPC_CREDENTIALS_H_
#define HUB_COMMON_GRPC_CREDENTIALS_H_

#include <memory>
#include <string>

#include <grpc++/grpc++.h>


namespace common {

std::string readFile(const std::string& fileName);
}

#endif  // HUB_COMMON_GRPC_CREDENTIALS_H_