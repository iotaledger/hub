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
  cmd::UserWithdraw withdrawCmd(session(), nullptr);

  constexpr auto username = "User1";
  constexpr auto toWithdraw = USER_BALANCE - 1;

  createUser(session(), username);

  withdrawRequest.set_userid(username);
  createBalanceForUsers({1}, USER_BALANCE);
  withdrawRequest.set_amount(toWithdraw);
  withdrawRequest.set_payoutaddress(
      "WLVZXWARPSYCWJMBZJGXHUOVYBVCEKMNQDMXHCAEJZFLFLMHFYYQQSSLVYWAZWESKXZOROLU"
      "9OQFRVDEWCHKKXPWGY");
  withdrawRequest.set_validatechecksum(true);

  auto status = withdrawCmd.doProcess(&withdrawRequest, &withdrawReply);

  ASSERT_EQ(status, common::cmd::OK);

  rpc::GetBalanceRequest balReq;
  rpc::GetBalanceReply balRep;
  rpc::Error err;

  balReq.set_userid(username);
  cmd::GetBalance balCommand(session());

  ASSERT_EQ(balCommand.doProcess(&balReq, &balRep), common::cmd::OK);

  ASSERT_EQ(USER_BALANCE - toWithdraw, balRep.available());

  rpc::UserWithdrawCancelRequest req;
  rpc::UserWithdrawCancelReply rep;
  cmd::UserWithdrawCancel cmd(session());

  std::string uu = withdrawReply.uuid();
  req.set_uuid(withdrawReply.uuid());
        ASSERT_EQ(cmd.doProcess(&req, &rep), common::cmd::OK);

  ASSERT_EQ(balCommand.doProcess(&balReq, &balRep), common::cmd::OK);

  ASSERT_EQ(USER_BALANCE, balRep.available());
}

TEST_F(UserWithdrawCancelTest, WithdrawalCancelOnlyOnce) {
  rpc::UserWithdrawRequest withdrawRequest;
  rpc::UserWithdrawReply withdrawReply;
  cmd::UserWithdraw withdrawCmd(session(), nullptr);

  constexpr auto username = "User1";
  constexpr auto toWithdraw = USER_BALANCE - 1;

  createUser(session(), username);

  withdrawRequest.set_userid(username);
  createBalanceForUsers({1}, USER_BALANCE);
  withdrawRequest.set_amount(toWithdraw);
  withdrawRequest.set_payoutaddress(
      "WLVZXWARPSYCWJMBZJGXHUOVYBVCEKMNQDMXHCAEJZFLFLMHFYYQQSSLVYWAZWESKXZOROLU"
      "9OQFRVDEWCHKKXPWGY");
  withdrawRequest.set_validatechecksum(true);

  auto status = withdrawCmd.doProcess(&withdrawRequest, &withdrawReply);

  ASSERT_EQ(status, common::cmd::OK);

  rpc::GetBalanceRequest balReq;
  rpc::GetBalanceReply balRep;
  rpc::Error err;

  balReq.set_userid(username);
  cmd::GetBalance balCommand(session());

  ASSERT_EQ(balCommand.doProcess(&balReq, &balRep), common::cmd::OK);

  ASSERT_EQ(USER_BALANCE - toWithdraw, balRep.available());

  rpc::UserWithdrawCancelRequest req;
  rpc::UserWithdrawCancelReply rep;
  cmd::UserWithdrawCancel cmd(session());

  std::string uu = withdrawReply.uuid();
  req.set_uuid(withdrawReply.uuid());
  ASSERT_EQ(cmd.doProcess(&req, &rep), common::cmd::OK);

  ASSERT_EQ(balCommand.doProcess(&balReq, &balRep), common::cmd::OK);

  ASSERT_EQ(USER_BALANCE, balRep.available());

  ASSERT_NE(cmd.doProcess(&req, &rep), common::cmd::OK);

  ASSERT_EQ(USER_BALANCE, balRep.available());
}

};  // namespace
