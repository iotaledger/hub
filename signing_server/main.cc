/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "signing_server/server.h"

int main(int argc, char** argv) {
  google::InstallFailureSignalHandler();

  auto programName = argc && argv && argc > 0 ? argv[0] : "unknown";
  google::InitGoogleLogging(programName);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  LOG(INFO) << "Signing server starting up.";

  signing::crypto::SigningServer server;

  server.initialize();
  server.runAndWait();
}
