#include "server.h"
#include "common/crypto/manager.h"

#include <chrono>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpc++/grpc++.h>

#include "common/common.h"
#include "common/crypto/argon2_provider.h"
#include "common/crypto/manager.h"
#include "common/flags.h"

using grpc::Server;
using grpc::ServerBuilder;
using namespace common::flags;

namespace signing {

namespace crypto {

void SigningServer::initialize() {
  if (common::flags::FLAGS_salt.size() <= 20) {
    LOG(FATAL) << "Salt must be more than 20 characters long.";
  }
  common::crypto::CryptoManager::get().setProvider(
      std::make_unique<common::crypto::Argon2Provider>(
          common::flags::FLAGS_salt));

  ServerBuilder builder;

  if (common::flags::FLAGS_authMode != "ssl") {
    LOG(FATAL) << "Remote signing_server should run only with credentials.";
  }

  builder.AddListeningPort(
      common::flags::FLAGS_listenAddress,
      makeCredentials(common::flags::FLAGS_authMode,
                      common::flags::FLAGS_sslCert, common::flags::FLAGS_sslKey,
                      common::flags::FLAGS_sslCA));
  builder.RegisterService(&_service);

  _server = builder.BuildAndStart();

  LOG(INFO) << "Server listening on " << common::flags::FLAGS_listenAddress;
}

}  // namespace crypto
}  // namespace signing
