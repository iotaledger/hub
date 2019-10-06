/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "hub/server/grpc_server.h"
#include "hub/server/http_json_server.h"

int main(int argc, char** argv) {
  google::InstallFailureSignalHandler();

  auto programName = argc && argv && argc > 0 ? argv[0] : "unknown";
  google::InitGoogleLogging(programName);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  LOG(INFO) << "Hub starting up.";

  std::shared_ptr<common::ServerBase> server;

  if (FLAGS_serverType == "http") {
    server.reset(new hub::HubHttpJsonServer());
  } else if (FLAGS_serverType == "grpc") {
    server.reset(new hub::HubGrpcServer());
  } else {
    LOG(FATAL) << "Unknown server type";
  }

  server->initialize();
  server->runAndWait();
}
