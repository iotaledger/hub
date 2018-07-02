/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/server/server.h"

#include <chrono>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpc++/grpc++.h>
#include "common/common.h"
#include "common/flags.h"
#include "cppclient/beast.h"
#include "hub/auth/dummy_provider.h"
#include "hub/auth/hmac_provider.h"
#include "hub/auth/manager.h"
#include "hub/crypto/argon2_provider.h"
#include "hub/crypto/client/remote_signing_provider.h"
#include "hub/crypto/manager.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/iota/pow.h"
#include "hub/iota/remote_pow.h"
#include "hub/service/sweep_service.h"
#include "hub/service/user_address_monitor.h"

DEFINE_string(apiAddress, "127.0.0.1:14265",
              "IRI node api to listen on. Format [host:port]");

DEFINE_uint32(monitorInterval, 60000, "Address monitor check interval [ms]");
DEFINE_uint32(attachmentInterval, 240000,
              "Attachment service check interval [ms]");
DEFINE_uint32(sweepInterval, 600000, "Sweep interval [ms]");

// TODO(th0br0): move to hub/iota/pow
DEFINE_uint32(minWeightMagnitude, 9, "Minimum weight magnitude for POW");
DEFINE_uint32(depth, 3, "Value for getTransacationToApprove depth parameter");

DEFINE_string(hmacKeyPath, "/dev/null", "path to key used for HMAC encyption");
DEFINE_string(authProvider, "none", "provider to use. can be {none, hmac}");

// remote crypto provider settings
DEFINE_string(signingMode, "local", "signing method to use {local,remote}");
DEFINE_string(
    signingProviderAddress, "0.0.0.0:50052",
    "crypto provider address, should be provided if remoteCryptoProvider=true");
// remote signing_server credentials
DEFINE_string(signingAuthMode, "none",
              "credentials to use. can be {none, ssl}");
// The following credentials components has to match those of the SigningServer
DEFINE_string(providerSslCert, "/dev/null",
              "Path to SSL certificate (ca.cert)");
DEFINE_string(providerChainCert, "/dev/null",
              "Path to SSL certificate chain (server.crt)");
DEFINE_string(providerKeyCert, "/dev/null",
              "Path to SSL certificate key (server.key)");

using grpc::Server;
using grpc::ServerBuilder;

namespace hub {
HubServer::HubServer() {}

void HubServer::initialise() {
  if (FLAGS_signingMode == "remote") {
    crypto::CryptoManager::get().setProvider(
        std::make_unique<crypto::RemoteSigningProvider>(
            FLAGS_signingProviderAddress, FLAGS_signingAuthMode,
            FLAGS_providerSslCert, FLAGS_providerChainCert,
            FLAGS_providerKeyCert));
  } else if (FLAGS_signingMode == "local") {
    if (common::flags::FLAGS_salt.size() <= 20) {
      LOG(FATAL) << "Salt must be more than 20 characters long.";
    }
    crypto::CryptoManager::get().setProvider(
        std::make_unique<crypto::Argon2Provider>(common::flags::FLAGS_salt));
  } else {
    LOG(FATAL) << "Signing mode: \"" << FLAGS_signingMode
               << "\" not recognized";
  }

  initialiseAuthProvider();

  db::DBManager::get().loadConnectionConfigFromArgs();

  if (!authenticateSalt()) {
    LOG(FATAL) << "The provided salt or provider parameters are not valid for "
                  "this database. Did you mistype?";
  }

  {
    size_t portIdx = FLAGS_apiAddress.find(':');
    auto host = FLAGS_apiAddress.substr(0, portIdx);
    auto port = std::stoi(FLAGS_apiAddress.substr(portIdx + 1));

    _api = std::make_shared<cppclient::BeastIotaAPI>(host, port);
  }

  iota::POWManager::get().setProvider(std::make_unique<iota::RemotePOW>(
      _api, FLAGS_depth, FLAGS_minWeightMagnitude));

  _userAddressMonitor = std::make_unique<service::UserAddressMonitor>(
      _api, std::chrono::milliseconds(FLAGS_monitorInterval));

  _attachmentService = std::make_unique<service::AttachmentService>(
      _api, std::chrono::milliseconds(FLAGS_attachmentInterval));
  _sweepService = std::make_unique<service::SweepService>(
      std::chrono::milliseconds(FLAGS_sweepInterval));

  ServerBuilder builder;

  builder.AddListeningPort(
      common::flags::FLAGS_listenAddress,
      makeCredentials(common::flags::FLAGS_authMode,
                      common::flags::FLAGS_sslCert, common::flags::FLAGS_sslKey,
                      common::flags::FLAGS_sslCA));
  builder.RegisterService(&_service);

  _server = builder.BuildAndStart();
  _userAddressMonitor->start();
  _attachmentService->start();
  _sweepService->start();

  LOG(INFO) << "Server listening on " << common::flags::FLAGS_listenAddress;
}

bool HubServer::authenticateSalt() const {
  auto& connection = db::DBManager::get().connection();
  auto addAndUuidRes = connection.selectFirstUserAddress();

  if (!addAndUuidRes.has_value()) return true;

  const auto& provider = crypto::CryptoManager::get().provider();
  const auto& [existantAddress, uuid] = addAndUuidRes.value();
  auto address = provider.getAddressForUUID(common::crypto::UUID(uuid));

  return address.str_view() == existantAddress;
}

void HubServer::initialiseAuthProvider() const {
  if (FLAGS_authProvider == "none") {
    auth::AuthManager::get().setProvider(
        std::make_unique<auth::DummyProvider>());
  } else if (FLAGS_authProvider == "hmac") {
    auth::AuthManager::get().setProvider(std::make_unique<auth::HMACProvider>(
        common::readFile(FLAGS_hmacKeyPath)));
  } else {
    LOG(FATAL) << "Unknown auth provider: " << FLAGS_authProvider;
  }
}

}  // namespace hub
