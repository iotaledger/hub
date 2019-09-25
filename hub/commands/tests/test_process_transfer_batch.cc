// Copyright 2018 IOTA Foundation

#include <map>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "hub/commands/create_user.h"
#include "hub/commands/get_balance.h"
#include "hub/commands/helper.h"
#include "hub/commands/process_transfer_batch.h"
#include "hub/commands/tests/helper.h"

#include "runner.h"

using namespace hub;
using namespace hub::tests;
using namespace sqlpp;

namespace {
static constexpr int64_t USER_BALANCE = 10000;
static constexpr int64_t MAX_TRANSFER_UNIT = 50;
static constexpr uint32_t NUM_USERS = 100;
static constexpr uint32_t NUM_TRANSFERS = 200;

class ProcessTransferBatchTest : public CommandTest {};

void processRandomTransfer(std::map<uint64_t, std::string>& idsToUsers,
                           std::map<uint64_t, int64_t>& idsToBalances,
                           std::shared_ptr<common::ClientSession> session,
                           bool threadSafe = false) {
  static std::mutex m;
  std::random_device rd;   // obtain a random number from hardware
  std::mt19937 eng(rd());  // seed the generator
  std::uniform_int_distribution<> distr(1,
                                        MAX_TRANSFER_UNIT);  // define the range
  std::vector<std::string> users;
  boost::copy(idsToUsers | boost::adaptors::map_values,
              std::inserter(users, users.begin()));
  cmd::ProcessTransferBatch cmd(std::move(session));

  for (uint32_t i = 0; i < NUM_TRANSFERS; ++i) {
    cmd::ProcessTransferBatchRequest req;
    cmd::ProcessTransferBatchReply res;

    auto idsToAmounts = createZigZagTransfer(users, req, distr(eng));
    bool cmdOK = false;
    try {
      auto status = cmd.process(&req, &res);
      cmdOK = (status == common::cmd::OK);
    } catch (const sqlpp::exception& ex) {
    }

    if (threadSafe) {
      if (cmdOK) {
        std::unique_lock<std::mutex> lock(m);
        for (auto& kv : idsToBalances) {
          idsToBalances[kv.first] += idsToAmounts[kv.first];
        }
      }
    } else {
      if (cmdOK) {
        for (auto& kv : idsToBalances) {
          idsToBalances[kv.first] += idsToAmounts[kv.first];
        }
      }
    }
  }
}

TEST_F(ProcessTransferBatchTest, FailOnNonExistingUserId) {
  cmd::ProcessTransferBatchRequest req;
  cmd::ProcessTransferBatchReply res;

  constexpr static auto userId = "Imaginary User";

  req.transfers.emplace_back(
      cmd::UserTransfer{.userId = userId, .amount = 100});
  req.transfers.emplace_back(
      cmd::UserTransfer{.userId = userId, .amount = -100});

  cmd::ProcessTransferBatch command(session());

  auto status = command.process(&req, &res);

  ASSERT_EQ(status, common::cmd::USER_DOES_NOT_EXIST);
}

TEST_F(ProcessTransferBatchTest, ZeroAmountTransferFails) {
  cmd::ProcessTransferBatchRequest req;
  cmd::ProcessTransferBatchReply res;

  auto userId = "User Id";
  auto status = createUser(session(), userId);
  ASSERT_TRUE(status == common::cmd::OK);

  req.transfers.emplace_back(cmd::UserTransfer{.userId = userId, .amount = 0});

  cmd::ProcessTransferBatch command(session());

  status = command.process(&req, &res);
  ASSERT_EQ(status, common::cmd::BATCH_INVALID);
}

TEST_F(ProcessTransferBatchTest, TransfersAreRecorded) {
  cmd::ProcessTransferBatchRequest req;
  cmd::ProcessTransferBatchReply res;
  std::vector<std::string> users;
  std::vector<uint64_t> userIds;
  cmd::ProcessTransferBatch command(session());
  int64_t absAmount = 10;

  for (auto i = 0; i < 10; ++i) {
    users.push_back("User " + std::to_string(i + 1));
    userIds.push_back(i + 1);  // incremental keys are predictable
    auto status = createUser(session(), users[i]);
    ASSERT_TRUE(status == common::cmd::OK);
  }
  createBalanceForUsers(userIds, USER_BALANCE);
  createZigZagTransfer(users, req, absAmount);
  auto status = command.process(&req, &res);

  ASSERT_TRUE(status == common::cmd::OK);

  cmd::GetBalance balCmd(session());
  cmd::GetBalanceRequest getBalReq;
  cmd::GetBalanceReply getBalRep;

  for (uint32_t i = 0; i < users.size(); ++i) {
    getBalReq.userId = users[i];
    int64_t amount = (i % 2) ? absAmount : -absAmount;
    ASSERT_TRUE(balCmd.process(&getBalReq, &getBalRep) == common::cmd::OK);
    ASSERT_EQ(USER_BALANCE + amount, getBalRep.available);
  }
}

TEST_F(ProcessTransferBatchTest, TransfersAreRecordedGroupingUserBalances) {
  cmd::ProcessTransferBatchRequest req;
  cmd::ProcessTransferBatchReply res;
  std::vector<std::string> users;
  std::vector<uint64_t> userIds;
  cmd::ProcessTransferBatch command(session());
  int64_t absAmount = 50;

  for (auto i = 0; i < 10; ++i) {
    users.push_back("User " + std::to_string(i + 1));
    userIds.push_back(i + 1);  // incremental keys are predictable
    auto status = createUser(session(), users[i]);
    ASSERT_TRUE(status == common::cmd::OK);
  }
  createBalanceForUsers(userIds, USER_BALANCE / 2);
  createBalanceForUsers(userIds, USER_BALANCE / 2);
  createZigZagTransfer(users, req, absAmount);
  auto status = command.process(&req, &res);

  ASSERT_TRUE(status == common::cmd::OK);

  cmd::GetBalance balCmd(session());
  cmd::GetBalanceRequest getBalReq;
  cmd::GetBalanceReply getBalRep;

  for (uint32_t i = 0; i < users.size(); ++i) {
    getBalReq.userId = users[i];
    int64_t amount = (i % 2) ? absAmount : -absAmount;
    ASSERT_TRUE(balCmd.process(&getBalReq, &getBalRep) == common::cmd::OK);
    ASSERT_EQ(USER_BALANCE + amount, getBalRep.available);
  }
}

TEST_F(ProcessTransferBatchTest, TransfersMustBeZeroSummed) {
  cmd::ProcessTransferBatchRequest req;
  cmd::ProcessTransferBatchReply res;
  std::vector<std::string> users;
  std::vector<uint64_t> userIds;
  cmd::ProcessTransferBatch command(session());

  for (auto i = 0; i < 9; ++i) {
    users.push_back("User " + std::to_string(i + 1));
    userIds.push_back(i + 1);  // incremental keys are predictable
    auto status = createUser(session(), users[i]);
    ASSERT_TRUE(status == common::cmd::OK);
  }
  createBalanceForUsers(userIds, USER_BALANCE);
  createZigZagTransfer(users, req, USER_BALANCE / 2);
  auto status = command.process(&req, &res);

  ASSERT_FALSE(status == common::cmd::OK);
  ASSERT_EQ(status, common::cmd::BATCH_AMOUNT_NOT_ZERO);
}

TEST_F(ProcessTransferBatchTest, TransferMustHaveSufficientFunds) {
  cmd::ProcessTransferBatchRequest req;
  cmd::ProcessTransferBatchReply res;
  std::vector<std::string> users;
  std::vector<uint64_t> userIds;
  cmd::ProcessTransferBatch command(session());

  for (auto i = 0; i < 2; ++i) {
    users.push_back("User " + std::to_string(i + 1));
    userIds.push_back(i + 1);  // incremental keys are predictable
    auto status = createUser(session(), users[i]);
    ASSERT_TRUE(status == common::cmd::OK);
  }
  createBalanceForUsers(userIds, USER_BALANCE);
  createZigZagTransfer(users, req, USER_BALANCE + 1);
  auto status = command.process(&req, &res);

  ASSERT_EQ(status, common::cmd::BATCH_INCONSISTENT);
}

TEST_F(ProcessTransferBatchTest, SequentialTransfersAreConsistent) {
  std::map<uint64_t, std::string> idsToUsers;
  std::map<uint64_t, int64_t> idsToBalances;

  for (uint32_t i = 0; i < NUM_USERS; ++i) {
    idsToUsers[i + 1] = "User " + std::to_string(i + 1);
    auto status = createUser(session(), idsToUsers[i + 1]);
    ASSERT_EQ(status, common::cmd::OK);
  }

  std::vector<uint64_t> userIds;
  boost::copy(idsToUsers | boost::adaptors::map_keys,
              std::inserter(userIds, userIds.begin()));

  idsToBalances = createBalanceForUsers(userIds, USER_BALANCE);

  for (auto i = 0; i < 10; ++i) {
    processRandomTransfer(idsToUsers, idsToBalances, session());
  }

  cmd::GetBalance balCmd(session());
  cmd::GetBalanceRequest getBalReq;
  cmd::GetBalanceReply getBalRep;

  for (auto& kv : idsToUsers) {
    getBalReq.userId = kv.second;
    ASSERT_EQ(balCmd.process(&getBalReq, &getBalRep), common::cmd::OK);
    ASSERT_EQ(idsToBalances[kv.first], getBalRep.available);
  }
}

TEST_F(ProcessTransferBatchTest, ConcurrentTransfersAreConsistent) {
  static constexpr uint32_t NUM_THREADS = 20;

  std::vector<std::thread> threads(NUM_THREADS);
  std::map<uint64_t, std::string> idsToUsers;
  std::map<uint64_t, int64_t> idsToBalances;

  for (uint32_t i = 0; i < NUM_USERS; ++i) {
    idsToUsers[i + 1] = "User " + std::to_string(i + 1);
    auto status = createUser(session(), idsToUsers[i + 1]);
    ASSERT_EQ(status, common::cmd::OK);
  }

  std::vector<uint64_t> userIds;
  boost::copy(idsToUsers | boost::adaptors::map_keys,
              std::inserter(userIds, userIds.begin()));
  idsToBalances = createBalanceForUsers(userIds, USER_BALANCE);

  for (uint32_t i = 0; i < NUM_THREADS; ++i) {
    std::thread t(processRandomTransfer, std::ref(idsToUsers),
                  std::ref(idsToBalances), session(), true);
    threads[i] = std::move(t);
  }

  for (auto& t : threads) {
    t.join();
  }

  cmd::GetBalance balCmd(session());
  cmd::GetBalanceRequest getBalReq;
  cmd::GetBalanceReply getBalRep;

  for (auto& kv : idsToUsers) {
    getBalReq.userId = kv.second;
    ASSERT_EQ(balCmd.process(&getBalReq, &getBalRep), common::cmd::OK);
    ASSERT_EQ(idsToBalances[kv.first], getBalRep.available);
  }
}

};  // namespace
