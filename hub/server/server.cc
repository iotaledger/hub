// Copyright 2018 IOTA Foundation

#include "hub/server/server.h"

#include <chrono>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpc++/grpc++.h>
#include "hub/crypto/local_provider.h"
#include "hub/crypto/manager.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/db/uuid.h"
#include "hub/iota/beast.h"
#include "hub/service/user_address_monitor.h"

DEFINE_string(salt, "", "Salt for local seed provider");
DEFINE_string(listenAddress, "0.0.0.0:50051", "address to listen on");
DEFINE_string(apiAddress, "127.0.0.1:14265",
              "IRI node api to listen on. Format [host:port]");
DEFINE_string(authMode, "none", "credentials to use. can be {none}");

DEFINE_uint32(monitorInterval, 60000, "Address monitor check interval [ms]");

using grpc::Server;
using grpc::ServerBuilder;

namespace hub {
HubServer::HubServer() {}

void HubServer::initialise() {
  if (FLAGS_salt.size() <= 20) {
    LOG(FATAL) << "Salt must be at least 10 characters long.";
  }
  crypto::CryptoManager::get().setProvider(
      std::make_unique<crypto::LocalProvider>(FLAGS_salt));

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

  _userAddressMonitor = std::make_unique<service::UserAddressMonitor>(
      _api, std::chrono::milliseconds(FLAGS_monitorInterval));

  ServerBuilder builder;

  builder.AddListeningPort(FLAGS_listenAddress,
                           grpc::InsecureServerCredentials());
  builder.RegisterService(&_service);

  _server = builder.BuildAndStart();
  _userAddressMonitor->start();

  LOG(INFO) << "Server listening on " << FLAGS_listenAddress;
}

void HubServer::runAndWait() { _server->Wait(); }

bool HubServer::authenticateSalt() const {
  auto& connection = db::DBManager::get().connection();
  auto addAndUuidRes = db::selectFirstUserAddress(connection);

  if (!addAndUuidRes.has_value()) return true;

  const auto& provider = crypto::CryptoManager::get().provider();
  const auto& [existantAddress, uuidStr] = addAndUuidRes.value();
  auto address = provider.getAddressForUUID(hub::db::uuidFromData(uuidStr));

  return address == existantAddress;
}

}  // namespace hub
