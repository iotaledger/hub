/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMON_SERVER_BASE_H_
#define HUB_COMMON_SERVER_BASE_H_

#include <memory>
#include <string>

#include <grpc++/grpc++.h>


namespace common {

class ServerBase {
 public:
  /// Creates and initializes the API interface.
  virtual void initialise() = 0;
  /// Runs the service and waits for requests
  virtual void runAndWait();

 protected:
  std::unique_ptr<grpc::Server> _server;
  std::shared_ptr<grpc::ServerCredentials> makeCredentials(
      const std::string& authMode, const std::string& sslCertPath,
      const std::string& sslKeyPath, const std::string& sslCAPath);
};
}  // namespace common


#endif  // HUB_COMMON_SERVER_BASE_H_
