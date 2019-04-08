// Copyright 2018 IOTA Foundation

#include <iostream>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

int main(int argc, char** argv) {
  google::InstallFailureSignalHandler();
  google::InitGoogleLogging("commands-tests");
  testing::InitGoogleMock(&argc, argv);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  return RUN_ALL_TESTS();
}
