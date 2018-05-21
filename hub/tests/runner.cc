// Copyright 2018 IOTA Foundation

#include "hub/tests/runner.h"

#include <iostream>

#include <glog/logging.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "hub/db/db.h"

int main(int argc, char** argv) {
  google::InstallFailureSignalHandler();
  google::InitGoogleLogging("commands-tests");
  testing::InitGoogleMock(&argc, argv);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  return RUN_ALL_TESTS();
}
