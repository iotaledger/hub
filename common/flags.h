/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_FLAGS_H_
#define COMMON_FLAGS_H_

#include <gflags/gflags.h>
#include <glog/logging.h>

namespace common {
namespace flags {
DECLARE_string(salt);
DECLARE_string(listenAddress);
DECLARE_string(sslCert);
DECLARE_string(sslKey);
DECLARE_string(sslCA);
DECLARE_string(authMode);
// Argon2
DECLARE_uint32(maxConcurrentArgon2Hash);
DECLARE_uint32(argon2TCost);
DECLARE_uint32(argon2MCost);
DECLARE_uint32(argon2Parallelism);
DECLARE_uint32(argon2Mode);
// IOTA seeds
DECLARE_uint32(keySecLevel);
// Http server
DECLARE_uint32(numThreadsHttp);
DECLARE_string(sslDH);
}  // namespace flags
}  // namespace common

#endif  // COMMON_FLAGS_H_