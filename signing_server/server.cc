#include "server.h"
#include "hub/crypto/manager.h"

#include <chrono>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpc++/grpc++.h>

#include "common/common.h"
#include "hub/crypto/argon2_provider.h"
#include "hub/crypto/manager.h"

DEFINE_string(salt, "", "Salt for argon2 seed provider");
DEFINE_string(listenAddress, "0.0.0.0:50052", "address to listen on");
DEFINE_string(authMode, "none", "credentials to use. can be {none, ssl}");
DEFINE_string(sslCert, "/dev/null", "Path to SSL certificate");
DEFINE_string(sslKey, "/dev/null", "Path to SSL certificate key");
DEFINE_string(sslCA, "/dev/null", "Path to CA root");

using grpc::Server;
using grpc::ServerBuilder;

namespace hub {

namespace crypto {

void SigningServer::initialise() {
  if (FLAGS_salt.size() <= 20) {
    LOG(FATAL) << "Salt must be more than 20 characters long.";
  }
  hub::crypto::CryptoManager::get().setProvider(
      std::make_unique<hub::crypto::Argon2Provider>(FLAGS_salt));

  ServerBuilder builder;

  builder.AddListeningPort(FLAGS_listenAddress,
                           makeCredentials(FLAGS_authMode, FLAGS_sslCert,
                                           FLAGS_sslKey, FLAGS_sslCA));
  builder.RegisterService(&_service);

  _server = builder.BuildAndStart();

  LOG(INFO) << "Server listening on " << FLAGS_listenAddress;
}

}  // namespace crypto
}  // namespace hub
