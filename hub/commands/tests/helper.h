// Copyright 2018 IOTA Foundation

#ifndef HUB_COMMANDS_TESTS_HELPER_H_
#define HUB_COMMANDS_TESTS_HELPER_H_

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "hub/commands/process_transfer_batch.h"
#include "hub/tests/runner.h"

namespace hub {

namespace tests {
std::map<uint64_t, int64_t> createBalanceForUsers(std::vector<uint64_t> ids,
                                                  int64_t balance);

std::map<uint64_t, int64_t> createZigZagTransfer(
    std::vector<std::string>& users, cmd::ProcessTransferBatchRequest& req,
    int64_t absAmount);

}  // namespace tests

}  // namespace hub
#endif  // HUB_COMMANDS_TESTS_HELPER_H_
