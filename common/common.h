/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_COMMON_H_
#define COMMON_COMMON_H_

#include <memory>
#include <string>

#include <grpc++/grpc++.h>

namespace common {

std::string readFile(const std::string& fileName);
}

#endif  // COMMON_COMMON_H_