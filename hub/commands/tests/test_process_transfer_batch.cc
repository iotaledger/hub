// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/create_user.h"
#include "hub/commands/get_balance.h"
#include "hub/commands/process_transfer_batch.h"
#include "hub/db/db.h"

#include "runner.h"

using namespace hub;
using namespace sqlpp;

namespace {
class ProcessTransferBatchTest : public CommandTest {};

TEST_F(ProcessTransferBatchTest, FailOnNonExistingUserId) {
  rpc::ProcessTransferBatchRequest req;
  rpc::ProcessTransferBatchReply res;

  auto* transfer = req.add_transfers();
  transfer->set_amount(100);
  transfer->set_userid("Imaginary User");

  cmd::ProcessTransferBatch command(session());

  auto status = command.doProcess(&req, &res);
  ASSERT_FALSE(status.ok());
}

TEST_F(ProcessTransferBatchTest, TransfersAreRecordedInDB) {
  rpc::ProcessTransferBatchRequest req;
  rpc::ProcessTransferBatchReply res;

  std::vector<std::string> users;
  for (auto i = 0; i < 5; ++i) {
    users.push_back("User " + std::to_string(i + 1));
    auto status = createUser(session(), users[i]);
    ASSERT_TRUE(status.ok());
    auto* transfer = req.add_transfers();
    transfer->set_amount(100);
    transfer->set_userid(users[i]);
  }

  cmd::ProcessTransferBatch command(session());

  auto status = command.doProcess(&req, &res);
  ASSERT_TRUE(status.ok());

  cmd::GetBalance balCmd(session());
  rpc::GetBalanceRequest getBalReq;
  rpc::GetBalanceReply getBalRep;

  for (auto i = 0; i < users.size(); ++i) {
    getBalReq.set_userid(users[i]);
    ASSERT_TRUE(balCmd.doProcess(&getBalReq, &getBalRep).ok());
    ASSERT_EQ(100, getBalRep.available());
  }
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
}

};  // namespace