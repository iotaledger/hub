/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/server/http_json_server.h"

#include "common/common.h"
#include "common/crypto/argon2_provider.h"
#include "common/crypto/manager.h"
#include "common/flags.h"
#include "cppclient/beast.h"
#include "hub/auth/dummy_provider.h"
#include "hub/auth/hmac_provider.h"
#include "hub/auth/manager.h"
#include "hub/commands/factory.h"
#include "hub/crypto/local_provider.h"
#include "hub/crypto/remote_signing_provider.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/iota/local_pow.h"
#include "hub/iota/pow.h"
#include "hub/iota/remote_pow.h"
#include "hub/service/sweep_service.h"
#include "hub/service/user_address_monitor.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <sstream>

namespace hub {
HubHttpJsonServer::HubHttpJsonServer() {}

void HubHttpJsonServer::initialize() {
  initialize_services();

  LOG(INFO) << "Hub http server was successfully initialized";
}

namespace pt = boost::property_tree;

common::HttpServerBase::ErrorCode HubHttpJsonServer::handleRequestImpl(
    std::string_view request_body, std::string& response) {
  // Create empty property tree object
  pt::ptree requestTree;
  pt::ptree responseTree;
  std::stringstream stream;

  std::stringstream jsonRequest;
  jsonRequest << request_body;

  // Parse the Json into the property tree.
  try {
    pt::read_json(jsonRequest, requestTree);
  } catch (
      boost::wrapexcept<boost::property_tree::json_parser::json_parser_error>
          ex) {
    LOG(ERROR) << "Wrong request format";
    responseTree.add("result", "Wrong request format");
    pt::write_json(stream, responseTree);
    response = stream.str();
    return common::HttpServerBase::ErrorCode::WRONG_REQUEST_FORMAT;
  }

  auto command = requestTree.get<std::string>("command");

  auto cmd = cmd::CommandFactory::get()->create(command, _api);
  if (cmd == nullptr) {
    LOG(ERROR) << "Unknown command";
    responseTree.add("result", "Unknown command");
    pt::write_json(stream, responseTree);
    response = stream.str();
    return common::HttpServerBase::ErrorCode::COMMAND_NOT_FOUND;
  }
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd->setClientSession(clientSession);
  responseTree = cmd->process(requestTree);

  pt::write_json(stream, responseTree);
  response = stream.str();
  return common::HttpServerBase::ErrorCode::OK;
}

}  // namespace hub
