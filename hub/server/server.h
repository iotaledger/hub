#ifndef __HUB_SERVER_H__
#define __HUB_SERVER_H__

#include <memory>

#include <grpc++/grpc++.h>

#include "hub/iota/api.h"
#include "hub/service/user_address_monitor.h"
#include "proto/hub.grpc.pb.h"
#include "proto/hub.pb.h"

#include "grpc.h"

namespace grpc {
class Server;
class ServerBuilder;
}  // namespace grpc

namespace hub {

class HubServer {
 public:
  explicit HubServer();

  void initialise();
  void runAndWait();

 private:
  std::unique_ptr<grpc::Server> _server;
  hub::HubImpl _service;

  std::shared_ptr<hub::iota::IotaAPI> _api;
  std::unique_ptr<hub::service::UserAddressMonitor> _userAddressMonitor;
};

}  // namespace hub
#endif /* __HUB_SERVER_H__ */
