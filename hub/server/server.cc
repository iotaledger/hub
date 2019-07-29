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
#include "common/crypto/argon2_provider.h"
#include "common/crypto/manager.h"
#include "common/flags.h"
#include "cppclient/beast.h"
#include "hub/auth/dummy_provider.h"
#include "hub/auth/hmac_provider.h"
#include "hub/auth/manager.h"
#include "hub/crypto/local_provider.h"
#include "hub/crypto/remote_signing_provider.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/iota/local_pow.h"
#include "hub/iota/pow.h"
#include "hub/iota/remote_pow.h"
#include "hub/service/sweep_service.h"
#include "hub/service/user_address_monitor.h"

DEFINE_string(apiAddress, "127.0.0.1:14265",
              "IRI node api to listen on. Format [host:port]");

DEFINE_uint32(monitorInterval, 60000,
              "Address monitor check interval (0=disabled) [ms]");
DEFINE_uint32(attachmentInterval, 240000,
              "Attachment service check interval (0=disabled) [ms]");
DEFINE_uint32(sweepInterval, 600000, "Sweep interval (0=disabled) [ms]");

DEFINE_uint32(minWeightMagnitude, 9, "Minimum weight magnitude for POW");
DEFINE_uint32(depth, 3, "Value for getTransacationToApprove depth parameter");

DEFINE_string(hmacKeyPath, "/dev/null", "path to key used for HMAC encyption");
DEFINE_string(authProvider, "none", "provider to use. can be {none, hmac}");

// remote crypto provider settings
DEFINE_string(signingMode, "local", "crypto method to use {local,remote}");
DEFINE_string(
    signingProviderAddress, "0.0.0.0:50052",
    "crypto provider address, should be provided if signingMode=remote");
// The following credentials components has to match those of the SigningServer
DEFINE_string(signingServerSslCert, "/dev/null",
              "Path to SSL certificate (ca.cert)");
DEFINE_string(signingServerChainCert, "/dev/null",
              "Path to SSL certificate chain (server.crt)");
DEFINE_string(signingServerKeyCert, "/dev/null",
              "Path to SSL certificate key (server.key)");

// remote/local pow settings
DEFINE_string(powMode, "remote", "PoW method to use {remote,local}");

// Extended
DEFINE_bool(fetchTransactionMessages, false,
            "Whether or not should hub fetch messages from transactions that "
            "are funding user addresses");

using grpc::Server;
using grpc::ServerBuilder;

namespace hub {
HubServer::HubServer() {}

void HubServer::initialise() {
  if (FLAGS_signingMode == "remote") {
    common::crypto::CryptoManager::get().setProvider(
        std::make_unique<crypto::RemoteSigningProvider>(
            FLAGS_signingProviderAddress, FLAGS_signingServerSslCert,
            FLAGS_signingServerChainCert, FLAGS_signingServerKeyCert));
  } else if (FLAGS_signingMode == "local") {
    if (common::flags::FLAGS_salt.size() <= 20) {
      LOG(FATAL) << "Salt must be more than 20 characters long.";
    }
    common::crypto::CryptoManager::get().setProvider(
        std::make_unique<hub::crypto::LocalSigningProvider>(
            common::flags::FLAGS_salt));
  } else {
    LOG(FATAL) << "Signing mode: \"" << FLAGS_signingMode
               << "\" not recognized";
  }

  initialiseAuthProvider();

  db::DBManager::get().loadConnectionConfigFromArgs();

  if (!authenticateSalt()) {
    if (FLAGS_signingMode == "remote") {
      LOG(FATAL) << "The remote crypto server could not be authenticated";
    } else {
      LOG(FATAL)
          << "The provided salt or provider parameters are not valid for "
             "this database. Did you mistype?";
    }
  }

  if (common::flags::FLAGS_keySecLevel > 3 ||
      common::flags::FLAGS_keySecLevel < 1) {
    LOG(FATAL)
        << "Key security level must be in the range [1,3], provided value: "
        << common::flags::FLAGS_keySecLevel;
  }

  {
    size_t portIdx = FLAGS_apiAddress.find(':');
    auto host = FLAGS_apiAddress.substr(0, portIdx);
    auto port = std::stoi(FLAGS_apiAddress.substr(portIdx + 1));

    _api = std::make_shared<cppclient::BeastIotaAPI>(host, port);
    _service.setApi(_api);
  }

  if (FLAGS_powMode == "remote") {
    iota::POWManager::get().setProvider(std::make_unique<iota::RemotePOW>(
        _api, FLAGS_depth, FLAGS_minWeightMagnitude));
  } else if (FLAGS_powMode == "local") {
    iota::POWManager::get().setProvider(std::make_unique<iota::LocalPOW>(
        FLAGS_depth, FLAGS_minWeightMagnitude));
  } else {
    LOG(FATAL) << "PoW mode: \"" << FLAGS_powMode << "\" not recognized";
  }

  _userAddressMonitor = std::make_unique<service::UserAddressMonitor>(
      _api, std::chrono::milliseconds(FLAGS_monitorInterval),
      FLAGS_fetchTransactionMessages);

  _attachmentService = std::make_unique<service::AttachmentService>(
      _api, std::chrono::milliseconds(FLAGS_attachmentInterval));
  _sweepService = std::make_unique<service::SweepService>(
      _api, std::chrono::milliseconds(FLAGS_sweepInterval));

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

  const auto& provider = common::crypto::CryptoManager::get().provider();
  const auto& [existantAddress, uuid] = addAndUuidRes.value();

  auto maybeAddress = provider.getAddressForUUID(common::crypto::UUID(uuid));
  if (!maybeAddress.has_value()) {
    LOG(ERROR) << "Could not get address from provider.";
    return false;
  }
  auto address = maybeAddress.value();

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
