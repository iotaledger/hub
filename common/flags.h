/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMON_FLAGS_H_
#define HUB_COMMON_FLAGS_H_

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
    }
}

#endif  // HUB_COMMON_FLAGS_H_