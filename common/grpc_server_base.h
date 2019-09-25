/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_GRPC_SERVER_BASE_H_
#define COMMON_GRPC_SERVER_BASE_H_

#include <memory>
#include <string>

#include "server_base.h"
#include "grpc++/grpc++.h"

namespace common {

class GrpcServerBase : public ServerBase  {
 public:
  /// Runs the service and waits for requests
  virtual void runAndWait() override ;

 protected:
  std::unique_ptr<grpc::Server> _server;
  std::shared_ptr<grpc::ServerCredentials> makeCredentials(
      const std::string& authMode, const std::string& sslCertPath,
      const std::string& sslKeyPath, const std::string& sslCAPath);
};
}  // namespace common

#endif  // COMMON_GRPC_SERVER_BASE_H_
