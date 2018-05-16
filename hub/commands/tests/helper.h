// Copyright 2018 IOTA Foundation

#ifndef HUB_COMMANDS_TESTS_HELPER_H_
#define HUB_COMMANDS_TESTS_HELPER_H_

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "hub/tests/runner.h"
#include "proto/hub.pb.h"

namespace hub {

namespace tests {

void createBalanceForUsers(std::vector<uint64_t> ids, int64_t balance);

std::map<uint64_t, int64_t> createZigZagTransfer(
    std::vector<std::string>& users, rpc::ProcessTransferBatchRequest& req,
    int64_t absAmount);

}  // namespace tests

}  // namespace hub
#endif  // HUB_COMMANDS_TESTS_HELPER_H_
