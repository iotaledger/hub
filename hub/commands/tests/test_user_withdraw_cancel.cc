// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include "hub/commands/get_balance.h"
#include "hub/commands/tests/helper.h"
#include "hub/commands/user_withdraw.h"
#include "hub/commands/user_withdraw_cancel.h"

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
  cmd::UserWithdrawRequest withdrawRequest;
  cmd::UserWithdrawReply withdrawReply;
  cmd::UserWithdraw withdrawCmd(session(), nullptr);

  constexpr auto username = "User1";
  constexpr auto toWithdraw = USER_BALANCE - 1;

  createUser(session(), username);

  withdrawRequest.userId = username;
  createBalanceForUsers({1}, USER_BALANCE);
  withdrawRequest.amount = toWithdraw;
  withdrawRequest.payoutAddress =
      "WLVZXWARPSYCWJMBZJGXHUOVYBVCEKMNQDMXHCAEJZFLFLMHFYYQQSSLVYWAZWESKXZOROLU"
      "9OQFRVDEWCHKKXPWGY";
  withdrawRequest.validateChecksum = true;

  auto status = withdrawCmd.doProcess(&withdrawRequest, &withdrawReply);

  ASSERT_EQ(status, common::cmd::OK);

  cmd::GetBalanceRequest balReq = {.userId = username};
  cmd::GetBalanceReply balRep;

  cmd::GetBalance balCommand(session());

  ASSERT_EQ(balCommand.doProcess(&balReq, &balRep), common::cmd::OK);

  ASSERT_EQ(USER_BALANCE - toWithdraw, balRep.available);

  cmd::UserWithdrawCancelRequest req;
  cmd::UserWithdrawCancelReply rep;
  cmd::UserWithdrawCancel cmd(session());

  std::string uu = withdrawReply.uuid;
  req.uuid = withdrawReply.uuid;
  ASSERT_EQ(cmd.doProcess(&req, &rep), common::cmd::OK);

  ASSERT_EQ(balCommand.doProcess(&balReq, &balRep), common::cmd::OK);

  ASSERT_EQ(USER_BALANCE, balRep.available);
}

TEST_F(UserWithdrawCancelTest, WithdrawalCancelOnlyOnce) {
  cmd::UserWithdrawRequest withdrawRequest;
  cmd::UserWithdrawReply withdrawReply;
  cmd::UserWithdraw withdrawCmd(session(), nullptr);

  constexpr auto username = "User1";
  constexpr auto toWithdraw = USER_BALANCE - 1;

  createUser(session(), username);

  withdrawRequest.userId = username;
  createBalanceForUsers({1}, USER_BALANCE);
  withdrawRequest.amount = toWithdraw;
  withdrawRequest.payoutAddress =
      "WLVZXWARPSYCWJMBZJGXHUOVYBVCEKMNQDMXHCAEJZFLFLMHFYYQQSSLVYWAZWESKXZOROLU"
      "9OQFRVDEWCHKKXPWGY";
  withdrawRequest.validateChecksum = true;

  auto status = withdrawCmd.doProcess(&withdrawRequest, &withdrawReply);

  ASSERT_EQ(status, common::cmd::OK);

  cmd::GetBalanceRequest balReq = {.userId = username};
  cmd::GetBalanceReply balRep;

  cmd::GetBalance balCommand(session());

  ASSERT_EQ(balCommand.doProcess(&balReq, &balRep), common::cmd::OK);

  ASSERT_EQ(USER_BALANCE - toWithdraw, balRep.available);

  cmd::UserWithdrawCancelRequest req;
  cmd::UserWithdrawCancelReply rep;
  cmd::UserWithdrawCancel cmd(session());

  std::string uu = withdrawReply.uuid;
  req.uuid = withdrawReply.uuid;
  ASSERT_EQ(cmd.doProcess(&req, &rep), common::cmd::OK);

  ASSERT_EQ(balCommand.doProcess(&balReq, &balRep), common::cmd::OK);

  ASSERT_EQ(USER_BALANCE, balRep.available);

  ASSERT_NE(cmd.doProcess(&req, &rep), common::cmd::OK);

  ASSERT_EQ(USER_BALANCE, balRep.available);
}

};  // namespace
