#ifndef __HUB_SERVER_H__
#define __HUB_SERVER_H__

#include <memory>

#include <grpc++/grpc++.h>

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
};

}  // namespace hub
#endif /* __HUB_SERVER_H__ */
