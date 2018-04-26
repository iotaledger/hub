// Copyright 2018 IOTA Foundation

#ifndef HUB_SERVER_SERVER_H_
#define HUB_SERVER_SERVER_H_

#include <memory>

#include <grpc++/grpc++.h>

#include "hub/iota/api.h"
#include "hub/server/grpc.h"
#include "hub/service/user_address_monitor.h"
#include "proto/hub.grpc.pb.h"
#include "proto/hub.pb.h"

namespace grpc {
class Server;
class ServerBuilder;
}  // namespace grpc

namespace hub {

class HubServer {
 public:
  HubServer();

  void initialise();
  void runAndWait();

 private:
  std::unique_ptr<grpc::Server> _server;
  hub::HubImpl _service;

  std::shared_ptr<hub::iota::IotaAPI> _api;
  std::unique_ptr<hub::service::UserAddressMonitor> _userAddressMonitor;
};

}  // namespace hub
#endif  // HUB_SERVER_SERVER_H_
