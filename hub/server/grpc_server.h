/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_SERVER_GRPC_SERVER_H_
#define HUB_SERVER_GRPC_SERVER_H_

#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "common/grpc_server_base.h"
#include "cppclient/api.h"
#include "hub/server/grpc.h"
#include "hub/service/attachment_service.h"
#include "hub/service/sweep_service.h"
#include "hub/service/user_address_monitor.h"
#include "hub_server_base.h"
#include "proto/hub.grpc.pb.h"
#include "proto/hub.pb.h"

namespace hub {

/// HubServer class.
/// The HubServer runs the local services and holds the instance of the
/// cppclient::IotaAPI complient API interface to the tangle. It also holds
/// an instance of the hub service that will accept requests
class HubGrpcServer : public common::GrpcServerBase, public HubServerBase {
 public:
  /// Constructor.
  HubGrpcServer();

  /// Creates and initializes the services and the API interface.
  void initialize() override;

 private:
  /// The hub::HubImpl service that listens to requests from clients
  hub::HubImpl _service;
};

}  // namespace hub
#endif  // HUB_SERVER_GRPC_SERVER_H_
