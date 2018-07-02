#include "server.h"
#include "hub/crypto/manager.h"

#include <chrono>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpc++/grpc++.h>

#include "common/common.h"
#include "common/flags.h"
#include "hub/crypto/argon2_provider.h"
#include "hub/crypto/manager.h"

using grpc::Server;
using grpc::ServerBuilder;
using namespace common::flags;

namespace signing {

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
}  // namespace signing
