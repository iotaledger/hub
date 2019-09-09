/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_SERVER_HTTP_SERVER_H_
#define HUB_SERVER_HTTP_SERVER_H_

#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "common/http_server_base.h"
#include "cppclient/api.h"
#include "hub/server/grpc.h"
#include "hub/service/attachment_service.h"
#include "hub/service/sweep_service.h"
#include "hub/service/user_address_monitor.h"

namespace hub {

/// HubServer class.
/// The HubServer runs the local services and holds the instance of the
/// cppclient::IotaAPI complient API interface to the tangle. It also holds
/// an instance of the hub service that will accept requests
class HubHttpServer : public common::HttpServerBase {
 public:
  /// Constructor.
  HubGrpcServer();

  /// Creates and initializes the services and the API interface.
  void initialise() override;

 private:
};

}  // namespace hub
#endif  // HUB_SERVER_HTTP_SERVER_H_
