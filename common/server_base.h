/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_SERVER_BASE_H_
#define COMMON_SERVER_BASE_H_

#include "common/flags.h"

namespace common {

    class ServerBase {
    public:
        /// Creates and initializes the API interface.
        virtual void initialize() = 0;
        /// Runs the service and waits for requests
        virtual void runAndWait() = 0;
    };
}  // namespace common

#endif  // COMMON_SERVER_BASE_H_
