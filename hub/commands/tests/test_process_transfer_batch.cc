// Copyright 2018 IOTA Foundation

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

static constexpr int64_t USER_BALANCE = 100;

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

};  // namespace