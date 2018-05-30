// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/get_balance.h"
#include "hub/commands/tests/helper.h"
#include "hub/commands/user_withdraw.h"
#include "hub/commands/user_withdraw_cancel.h"
#include "hub/db/db.h"

#include "runner.h"

using namespace hub;
using namespace hub::tests;
using namespace sqlpp;

static constexpr int64_t USER_BALANCE = 100;

// check that balance updates after cancel
// should check that a withdrawal can't be processed in sweep after it was
// cancelled

namespace {
class UserWithdrawCancelTest : public CommandTest {};

TEST_F(UserWithdrawCancelTest, WithdrawalCancelUpdatesUserBalance) {
  rpc::UserWithdrawRequest withdrawRequest;
  rpc::UserWithdrawReply withdrawReply;
  cmd::UserWithdraw withdrawCmd(session());

  constexpr auto username = "User1";
  constexpr auto toWithdraw = USER_BALANCE - 1;

  createUser(session(), username);

  withdrawRequest.set_userid(username);
  createBalanceForUsers({1}, USER_BALANCE);
  withdrawRequest.set_amount(toWithdraw);
  withdrawRequest.set_payoutaddress(
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999");

  auto status = withdrawCmd.doProcess(&withdrawRequest, &withdrawReply);

  ASSERT_TRUE(status.ok());

  rpc::GetBalanceRequest balReq;
  rpc::GetBalanceReply balRep;
  rpc::Error err;

  balReq.set_userid(username);
  cmd::GetBalance balCommand(session());

  ASSERT_TRUE(balCommand.doProcess(&balReq, &balRep).ok());

  ASSERT_EQ(USER_BALANCE - toWithdraw, balRep.available());

  rpc::UserWithdrawCancelRequest req;
  rpc::UserWithdrawCancelReply rep;
  cmd::UserWithdrawCancel cmd(session());

  std::string uu = withdrawReply.uuid();
  req.set_uuid(withdrawReply.uuid());
  ASSERT_TRUE(cmd.doProcess(&req, &rep).ok());

  ASSERT_TRUE(balCommand.doProcess(&balReq, &balRep).ok());

  ASSERT_EQ(USER_BALANCE, balRep.available());
}

TEST_F(UserWithdrawCancelTest, WithdrawalCancelOnlyOnce) {
  rpc::UserWithdrawRequest withdrawRequest;
  rpc::UserWithdrawReply withdrawReply;
  cmd::UserWithdraw withdrawCmd(session());

  constexpr auto username = "User1";
  constexpr auto toWithdraw = USER_BALANCE - 1;

  createUser(session(), username);

  withdrawRequest.set_userid(username);
  createBalanceForUsers({1}, USER_BALANCE);
  withdrawRequest.set_amount(toWithdraw);
  withdrawRequest.set_payoutaddress(
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999");

  auto status = withdrawCmd.doProcess(&withdrawRequest, &withdrawReply);

  ASSERT_TRUE(status.ok());

  rpc::GetBalanceRequest balReq;
  rpc::GetBalanceReply balRep;
  rpc::Error err;

  balReq.set_userid(username);
  cmd::GetBalance balCommand(session());

  ASSERT_TRUE(balCommand.doProcess(&balReq, &balRep).ok());

  ASSERT_EQ(USER_BALANCE - toWithdraw, balRep.available());

  rpc::UserWithdrawCancelRequest req;
  rpc::UserWithdrawCancelReply rep;
  cmd::UserWithdrawCancel cmd(session());

  std::string uu = withdrawReply.uuid();
  req.set_uuid(withdrawReply.uuid());
  ASSERT_TRUE(cmd.doProcess(&req, &rep).ok());

  ASSERT_TRUE(balCommand.doProcess(&balReq, &balRep).ok());

  ASSERT_EQ(USER_BALANCE, balRep.available());

  ASSERT_FALSE(cmd.doProcess(&req, &rep).ok());

  ASSERT_EQ(USER_BALANCE, balRep.available());
}

};  // namespace
