/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_ARGON_FLAGS_H
#define HUB_ARGON_FLAGS_H

#include <gflags/gflags.h>

DECLARE_uint32(maxConcurrentArgon2Hash);
DECLARE_uint32(argon2TCost);
DECLARE_uint32(argon2MCost);
DECLARE_uint32(argon2Parallelism);
DECLARE_uint32(argon2Mode);

#endif  // HUB_ARGON_FLAGS_H
