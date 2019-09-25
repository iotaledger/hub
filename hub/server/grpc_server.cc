/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/server/grpc_server.h"

#include <glog/logging.h>
#include <grpc++/grpc++.h>
#include <chrono>
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

using grpc::Server;
using grpc::ServerBuilder;

namespace hub {
HubGrpcServer::HubGrpcServer() {}

void HubGrpcServer::initialize() {
  initialize_services();

  _service.setApi(_api);

  ServerBuilder builder;

  builder.AddListeningPort(
      common::flags::FLAGS_listenAddress,
      makeCredentials(common::flags::FLAGS_authMode,
                      common::flags::FLAGS_sslCert, common::flags::FLAGS_sslKey,
                      common::flags::FLAGS_sslCA));
  builder.RegisterService(&_service);

  _server = builder.BuildAndStart();

  LOG(INFO) << "Server listening on " << common::flags::FLAGS_listenAddress;
}

}  // namespace hub
