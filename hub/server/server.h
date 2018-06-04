/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_SERVER_SERVER_H_
#define HUB_SERVER_SERVER_H_

#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "hub/iota/api.h"
#include "hub/server/grpc.h"
#include "hub/service/attachment_service.h"
#include "hub/service/sweep_service.h"
#include "hub/service/user_address_monitor.h"
#include "proto/hub.grpc.pb.h"
#include "proto/hub.pb.h"

namespace grpc {
class Server;
class ServerBuilder;
class ServerCredentials;
}  // namespace grpc

namespace hub {

/// HubServer class.
/// The HubServer runs the local services and holds the instance of the
/// hub::iota::IotaAPI complient API interface to the tangle. It also holds
/// an instance of the hub service that will accept requests
class HubServer {
 public:
  /// Constructor.
  HubServer();

  /// Creates and initializes the services and the API interface.
  void initialise();
  /// Runs the hub service and waits for requests
  void runAndWait();

 private:
  std::unique_ptr<grpc::Server> _server;
  /// The hub::HubImpl service that listens to requests from clients
  hub::HubImpl _service;

  std::shared_ptr<hub::iota::IotaAPI> _api;
  std::unique_ptr<hub::service::UserAddressMonitor> _userAddressMonitor;
  std::unique_ptr<hub::service::AttachmentService> _attachmentService;
  std::unique_ptr<hub::service::SweepService> _sweepService;

  /// Verifies that the salt provided on the command line has the correct
  /// value for this server. Will always succeed if no user addresses exist
  /// in the database.
  /// @return bool - true on success
  bool authenticateSalt() const;

  std::shared_ptr<grpc::ServerCredentials> makeCredentials();
};

}  // namespace hub
#endif  // HUB_SERVER_SERVER_H_
