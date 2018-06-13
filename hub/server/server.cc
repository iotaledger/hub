/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/server/server.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpc++/grpc++.h>
#include "hub/auth/dummy_provider.h"
#include "hub/auth/manager.h"
#include "hub/crypto/local_provider.h"
#include "hub/crypto/manager.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/iota/beast.h"
#include "hub/iota/pow.h"
#include "hub/iota/remote_pow.h"
#include "hub/service/sweep_service.h"
#include "hub/service/user_address_monitor.h"

DEFINE_string(salt, "", "Salt for local seed provider");
DEFINE_string(listenAddress, "0.0.0.0:50051", "address to listen on");
DEFINE_string(apiAddress, "127.0.0.1:14265",
              "IRI node api to listen on. Format [host:port]");

DEFINE_uint32(monitorInterval, 60000, "Address monitor check interval [ms]");
DEFINE_uint32(attachmentInterval, 240000,
              "Attachment service check interval [ms]");
DEFINE_uint32(sweepInterval, 600000, "Sweep interval [ms]");

// TODO(th0br0): move to hub/iota/pow
DEFINE_uint32(minWeightMagnitude, 9, "Minimum weight magnitude for POW");
DEFINE_uint32(depth, 3, "Value for getTransacationToApprove depth parameter");

DEFINE_string(authMode, "none", "credentials to use. can be {none, ssl}");
DEFINE_string(sslCert, "/dev/null", "Path to SSL certificate");
DEFINE_string(sslKey, "/dev/null", "Path to SSL certificate key");
DEFINE_string(sslCA, "/dev/null", "Path to CA root");

using grpc::Server;
using grpc::ServerBuilder;

namespace {
std::string readFile(const std::string& fileName) {
  std::ifstream ifs(fileName.c_str());
  std::stringstream buffer;

  buffer << ifs.rdbuf();
  return buffer.str();
}

}  // namespace

namespace hub {
HubServer::HubServer() {}

std::shared_ptr<grpc::ServerCredentials> HubServer::makeCredentials() {
  LOG(INFO) << "Using auth mode: " << FLAGS_authMode;
  if (FLAGS_authMode == "none") {
    return grpc::InsecureServerCredentials();
  } else if (FLAGS_authMode == "ssl") {
    grpc::SslServerCredentialsOptions options;

    grpc::SslServerCredentialsOptions::PemKeyCertPair keycert = {
        readFile(FLAGS_sslKey), readFile(FLAGS_sslCert)};

    options.pem_key_cert_pairs.push_back(keycert);
    options.pem_root_certs = readFile(FLAGS_sslCA);
    options.client_certificate_request =
        GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY;
    return grpc::SslServerCredentials(options);
  }

  LOG(FATAL) << "Unknown auth mode: " << FLAGS_authMode;
}

void HubServer::initialise() {
  if (FLAGS_salt.size() <= 20) {
    LOG(FATAL) << "Salt must be more than 20 characters long.";
  }
  crypto::CryptoManager::get().setProvider(
      std::make_unique<crypto::LocalProvider>(FLAGS_salt));

  auth::AuthManager::get().setProvider(std::make_unique<auth::DummyProvider>());

  db::DBManager::get().loadConnectionConfigFromArgs();

  if (!authenticateSalt()) {
    LOG(FATAL) << "The provided salt is not valid for "
                  "this database. Did you mistype?";
  }

  {
    size_t portIdx = FLAGS_apiAddress.find(':');
    auto host = FLAGS_apiAddress.substr(0, portIdx);
    auto port = std::stoi(FLAGS_apiAddress.substr(portIdx + 1));

    _api = std::make_shared<iota::BeastIotaAPI>(host, port);
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

  builder.AddListeningPort(FLAGS_listenAddress, makeCredentials());
  builder.RegisterService(&_service);

  _server = builder.BuildAndStart();
  _userAddressMonitor->start();
  _attachmentService->start();
  _sweepService->start();

  LOG(INFO) << "Server listening on " << FLAGS_listenAddress;
}

void HubServer::runAndWait() { _server->Wait(); }

bool HubServer::authenticateSalt() const {
  auto& connection = db::DBManager::get().connection();
  auto addAndUuidRes = connection.selectFirstUserAddress();

  if (!addAndUuidRes.has_value()) return true;

  const auto& provider = crypto::CryptoManager::get().provider();
  const auto& [existantAddress, uuid] = addAndUuidRes.value();
  auto address = provider.getAddressForUUID(crypto::UUID(uuid));

  return address.str_view() == existantAddress;
}

}  // namespace hub
