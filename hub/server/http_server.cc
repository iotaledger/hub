/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/server/http_server.h"

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

namespace hub {
HubHttpServer::HubHttpServer() {}

void HubHttpServer::initialise() {
  initialise_services();

  LOG(INFO) << "Hub http server was successfully initialized";
}

common::HttpServerBase::ErrorCode HubHttpServer::handleRequestImpl(
    std::string_view request_body, std::string& response) {
  response = "Not implemented\n";
  return common::HttpServerBase::ErrorCode::OK;
}

}  // namespace hub
