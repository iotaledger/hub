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

namespace iota {

class HubServer {
 public:
  explicit HubServer();

  void initialise();
  void runAndWait();

 private:
  std::unique_ptr<grpc::Server> _server;
  iota::HubImpl _service;
};

}  // namespace iota
#endif /* __HUB_SERVER_H__ */
