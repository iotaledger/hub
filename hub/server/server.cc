#include "server.h"

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpc++/grpc++.h>

DEFINE_string(listenAddress, "0.0.0.0:50051", "address to listen on");
DEFINE_string(authMode, "none", "credentials to use. can be {none}");

using grpc::Server;
using grpc::ServerBuilder;

namespace hub {
HubServer::HubServer() {}

void HubServer::initialise() {
  ServerBuilder builder;

  builder.AddListeningPort(FLAGS_listenAddress,
                           grpc::InsecureServerCredentials());
  builder.RegisterService(&_service);

  _server = builder.BuildAndStart();

  LOG(INFO) << "Server listening on " << FLAGS_listenAddress;
}

void HubServer::runAndWait() { _server->Wait(); }
}  // namespace hub
