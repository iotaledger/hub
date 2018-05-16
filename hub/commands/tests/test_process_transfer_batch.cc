// Copyright 2018 IOTA Foundation

#include <thread>
#include <random>
#include <vector>
#include <map>
#include <mutex>

#include <gtest/gtest.h>


#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/create_user.h"
#include "hub/commands/get_balance.h"
#include "hub/commands/helper.h"
#include "hub/commands/process_transfer_batch.h"
#include "hub/commands/tests/helper.h"
#include "hub/db/db.h"

#include "runner.h"

using namespace hub;
using namespace hub::tests;
using namespace sqlpp;

namespace {
class ProcessTransferBatchTest : public CommandTest {};

static constexpr int64_t USER_BALANCE = 10000;
static constexpr int64_t MAX_TRANSFER_UNIT = 50;
std::mutex m;

void processRandomTransfer(std::vector<std::string> users,
                           std::map<uint64_t, int64_t>& idsToBalances,
                           std::shared_ptr<hub::ClientSession> session) {
  std::random_device rd;   // obtain a random number from hardware
  std::mt19937 eng(rd());  // seed the generator
  std::uniform_int_distribution<> distr(1,
                                        MAX_TRANSFER_UNIT);  // define the range

  for (auto i = 0; i < 10; ++i) {
    rpc::ProcessTransferBatchRequest req;
    rpc::ProcessTransferBatchReply res;
    cmd::ProcessTransferBatch cmd(std::move(session));
    auto idsToAmounts = createZigZagTransfer(users, req, distr(eng));
    auto status = cmd.doProcess(&req, &res);

    ASSERT_TRUE(status.ok());

    for (auto& kv : idsToBalances) {
      idsToBalances[kv.first] -= idsToAmounts[kv.first];
    }
  }
}
void processRandomTransferLock(std::vector<std::string> users,
                               std::map<uint64_t, int64_t>& idsToBalances,
                               std::shared_ptr<hub::ClientSession> session) {
  std::random_device rd;   // obtain a random number from hardware
  std::mt19937 eng(rd());  // seed the generator
  std::uniform_int_distribution<> distr(1,
                                        MAX_TRANSFER_UNIT);  // define the range
  std::unique_lock<std::mutex> lock(m);

  cmd::ProcessTransferBatch cmd(std::move(session));

  for (auto i = 0; i < 10; ++i) {
    rpc::ProcessTransferBatchRequest req;
    rpc::ProcessTransferBatchReply res;

    auto idsToAmounts = createZigZagTransfer(users, req, distr(eng));
    auto status = cmd.doProcess(&req, &res);
    if (status.ok()) {
      for (auto& kv : idsToBalances) {
        idsToBalances[kv.first] -= idsToAmounts[kv.first];
      }
    }
  }
}

TEST_F(ProcessTransferBatchTest, FailOnNonExistingUserId) {
  rpc::ProcessTransferBatchRequest req;
  rpc::ProcessTransferBatchReply res;

  auto* transfer = req.add_transfers();
  transfer->set_amount(100);
  transfer->set_userid("Imaginary User");

  transfer = req.add_transfers();
  transfer->set_amount(-100);
  transfer->set_userid("Imaginary User");

  cmd::ProcessTransferBatch command(session());

  auto status = command.doProcess(&req, &res);

  ASSERT_FALSE(status.ok());
  ASSERT_EQ(status.error_message(),
            cmd::errorToString(hub::rpc::ErrorCode::USER_DOES_NOT_EXIST));
}

TEST_F(ProcessTransferBatchTest, ZeroAmountTransferFails) {
  rpc::ProcessTransferBatchRequest req;
  rpc::ProcessTransferBatchReply res;

  auto userId = "User Id";
  auto status = createUser(session(), userId);
  ASSERT_TRUE(status.ok());

  auto* transfer = req.add_transfers();
  transfer->set_amount(0);
  transfer->set_userid(userId);

  cmd::ProcessTransferBatch command(session());

  status = command.doProcess(&req, &res);
  ASSERT_FALSE(status.ok());
  ASSERT_EQ(status.error_message(),
            cmd::errorToString(hub::rpc::ErrorCode::BATCH_INVALID));
}

TEST_F(ProcessTransferBatchTest, TransfersAreRecorded) {
  rpc::ProcessTransferBatchRequest req;
  rpc::ProcessTransferBatchReply res;
  std::vector<std::string> users;
  std::vector<uint64_t> userIds;
  cmd::ProcessTransferBatch command(session());
  int64_t absAmount = 10;

  for (auto i = 0; i < 10; ++i) {
    users.push_back("User " + std::to_string(i + 1));
    userIds.push_back(i + 1);  // incremental keys are predictable
    auto status = createUser(session(), users[i]);
    ASSERT_TRUE(status.ok());
  }
  createBalanceForUsers(userIds, USER_BALANCE);
  createZigZagTransfer(users, req, absAmount);
  auto status = command.doProcess(&req, &res);

  ASSERT_TRUE(status.ok());

  cmd::GetBalance balCmd(session());
  rpc::GetBalanceRequest getBalReq;
  rpc::GetBalanceReply getBalRep;

  for (auto i = 0; i < users.size(); ++i) {
    getBalReq.set_userid(users[i]);
    int64_t amount = (i % 2) ? absAmount : -absAmount;
    ASSERT_TRUE(balCmd.doProcess(&getBalReq, &getBalRep).ok());
    ASSERT_EQ(USER_BALANCE + amount, getBalRep.available());
  }
}

TEST_F(ProcessTransferBatchTest, TransfersAreRecordedGroupingUserBalances) {
  rpc::ProcessTransferBatchRequest req;
  rpc::ProcessTransferBatchReply res;
  std::vector<std::string> users;
  std::vector<uint64_t> userIds;
  cmd::ProcessTransferBatch command(session());
  int64_t absAmount = 50;

  for (auto i = 0; i < 10; ++i) {
    users.push_back("User " + std::to_string(i + 1));
    userIds.push_back(i + 1);  // incremental keys are predictable
    auto status = createUser(session(), users[i]);
    ASSERT_TRUE(status.ok());
  }
  createBalanceForUsers(userIds, USER_BALANCE / 2);
  createBalanceForUsers(userIds, USER_BALANCE / 2);
  createZigZagTransfer(users, req, absAmount);
  auto status = command.doProcess(&req, &res);

  ASSERT_TRUE(status.ok());

  cmd::GetBalance balCmd(session());
  rpc::GetBalanceRequest getBalReq;
  rpc::GetBalanceReply getBalRep;

  for (auto i = 0; i < users.size(); ++i) {
    getBalReq.set_userid(users[i]);
    int64_t amount = (i % 2) ? absAmount : -absAmount;
    ASSERT_TRUE(balCmd.doProcess(&getBalReq, &getBalRep).ok());
    ASSERT_EQ(USER_BALANCE + amount, getBalRep.available());
  }
}

TEST_F(ProcessTransferBatchTest, TransfersMustBeZeroSummed) {
  rpc::ProcessTransferBatchRequest req;
  rpc::ProcessTransferBatchReply res;
  std::vector<std::string> users;
  std::vector<uint64_t> userIds;
  cmd::ProcessTransferBatch command(session());

  for (auto i = 0; i < 9; ++i) {
    users.push_back("User " + std::to_string(i + 1));
    userIds.push_back(i + 1);  // incremental keys are predictable
    auto status = createUser(session(), users[i]);
    ASSERT_TRUE(status.ok());
  }
  createBalanceForUsers(userIds, USER_BALANCE);
  createZigZagTransfer(users, req, USER_BALANCE / 2);
  auto status = command.doProcess(&req, &res);

  ASSERT_FALSE(status.ok());
  ASSERT_EQ(status.error_message(),
            cmd::errorToString(hub::rpc::ErrorCode::BATCH_AMOUNT_ZERO));
}

TEST_F(ProcessTransferBatchTest, TransferMustHaveSufficientFunds) {
  rpc::ProcessTransferBatchRequest req;
  rpc::ProcessTransferBatchReply res;
  std::vector<std::string> users;
  std::vector<uint64_t> userIds;
  cmd::ProcessTransferBatch command(session());

  for (auto i = 0; i < 2; ++i) {
    users.push_back("User " + std::to_string(i + 1));
    userIds.push_back(i + 1);  // incremental keys are predictable
    auto status = createUser(session(), users[i]);
    ASSERT_TRUE(status.ok());
  }
  createBalanceForUsers(userIds, USER_BALANCE);
  createZigZagTransfer(users, req, USER_BALANCE + 1);
  auto status = command.doProcess(&req, &res);

  ASSERT_FALSE(status.ok());
  ASSERT_EQ(status.error_message(),
            cmd::errorToString(hub::rpc::ErrorCode::BATCH_INCONSISTENT));
}

TEST_F(ProcessTransferBatchTest, SequentialTransfersAreConsistent) {
  std::vector<std::string> users;
  std::vector<uint64_t> userIds;

  std::map<uint64_t, int64_t> idsToBalances;

  constexpr uint32_t numUsers = 100;
  constexpr uint32_t numTransfers = 20;

  for (auto i = 0; i < numUsers; ++i) {
    users.push_back("User " + std::to_string(i + 1));
    userIds.push_back(i + 1);  // incremental keys are predictable
    auto status = createUser(session(), users[i]);
    ASSERT_TRUE(status.ok());
    idsToBalances[i + 1] = USER_BALANCE;
  }
  createBalanceForUsers(userIds, USER_BALANCE);

  for (auto i = 0; i < 20; ++i) {
    processRandomTransfer(users, idsToBalances, session());
  }

  cmd::GetBalance balCmd(session());
  rpc::GetBalanceRequest getBalReq;
  rpc::GetBalanceReply getBalRep;

  for (auto i = 0; i < users.size() - 1; ++i) {
    getBalReq.set_userid(users[i]);
    ASSERT_TRUE(balCmd.doProcess(&getBalReq, &getBalRep).ok());
    ASSERT_EQ(idsToBalances[userIds[i]], getBalRep.available());
  }
}

TEST_F(ProcessTransferBatchTest, ConcurrentTransfersAreConsistent) {
  static constexpr uint32_t NUM_THREADS = 3;
  std::vector<std::string> users;
  std::vector<uint64_t> userIds;
  std::vector<std::thread> threads(NUM_THREADS);

  std::map<uint64_t, int64_t> idsToBalances;

  constexpr uint32_t numUsers = 100;
  constexpr uint32_t numTransfers = 20;

  for (auto i = 0; i < numUsers; ++i) {
    users.push_back("User " + std::to_string(i + 1));
    userIds.push_back(i + 1);  // incremental keys are predictable
    auto status = createUser(session(), users[i]);
    ASSERT_TRUE(status.ok());
    idsToBalances[i + 1] = USER_BALANCE;
  }
  createBalanceForUsers(userIds, USER_BALANCE);

  for (auto i = 0; i < NUM_THREADS; ++i) {
    std::thread t(processRandomTransferLock, users, std::ref(idsToBalances),
                  session());
    threads[i] = std::move(t);
  }

  for (auto& t : threads) {
    t.join();
  }

  cmd::GetBalance balCmd(session());
  rpc::GetBalanceRequest getBalReq;
  rpc::GetBalanceReply getBalRep;

  for (auto i = 0; i < users.size() - 1; ++i) {
    getBalReq.set_userid(users[i]);
    ASSERT_TRUE(balCmd.doProcess(&getBalReq, &getBalRep).ok());
    ASSERT_EQ(idsToBalances[userIds[i]], getBalRep.available());
  }
}

};  // namespace
