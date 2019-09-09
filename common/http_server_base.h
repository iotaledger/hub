/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_HTTP_SERVER_BASE_H_
#define COMMON_HTTP_SERVER_BASE_H_

#include <memory>
#include <string>

#include "server_base.h"

namespace common {

    class HttpServerBase : public ServerBase  {
    public:
        /// Runs the service and waits for requests
        virtual void runAndWait() override ;
    };
}  // namespace common

#endif  // COMMON_HTTP_SERVER_BASE_H_
