// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include "hub/commands/get_balance.h"
#include "hub/commands/tests/helper.h"
#include "hub/commands/user_withdraw.h"

#include "runner.h"

using namespace hub;
using namespace hub::tests;
using namespace sqlpp;

namespace {
class UserWithdrawTest : public CommandTest {};

TEST_F(UserWithdrawTest, ErrorOnInvalidPayoutAddress) {
  cmd::UserWithdrawRequest req;
  cmd::UserWithdrawReply res;
  cmd::UserWithdraw command(session(), nullptr);

  createUser(session(), "a");

  req.userId = "a";
  req.amount = 0;
  req.validateChecksum = true;
  req.payoutAddress = "999999999";
  ASSERT_NE(command.doProcess(&req, &res), common::cmd::OK);
}

TEST_F(UserWithdrawTest, ErrorOnZeroAmount) {
  cmd::UserWithdrawRequest req;
  cmd::UserWithdrawReply res;
  cmd::UserWithdraw command(session(), nullptr);

  createUser(session(), "a");

  req.userId = "a";
  req.amount = 0;
  req.validateChecksum = false;
  req.payoutAddress =
      "WLVZXWARPSYCWJMBZJGXHUOVYBVCEKMNQDMXHCAEJZFLFLMHFYYQQSSLVYWAZWESKXZOROLU"
      "9OQFRVDEWCHKKXPWGY";

  auto status = command.process(&req, &res);

  ASSERT_NE(status, common::cmd::OK);
}

TEST_F(UserWithdrawTest, WithdrawalUpdatesUserBalance) {
  cmd::UserWithdrawRequest req;
  cmd::UserWithdrawReply res;
  cmd::UserWithdraw command(session(), nullptr);

  constexpr auto username = "User1";

  createUser(session(), username);

  req.userId = username;
  createBalanceForUsers({1}, 100);
  req.amount = 50;
  req.validateChecksum = true;
  req.payoutAddress =
      "WLVZXWARPSYCWJMBZJGXHUOVYBVCEKMNQDMXHCAEJZFLFLMHFYYQQSSLVYWAZWESKXZOROLU"
      "9OQFRVDEWCHKKXPWGY";

  auto status = command.doProcess(&req, &res);

  ASSERT_EQ(status, common::cmd::OK);

  cmd::GetBalanceRequest balReq = {.userId = username};
  cmd::GetBalanceReply balRes;

  cmd::GetBalance balCommand(session());

  ASSERT_EQ(balCommand.doProcess(&balReq, &balRes), common::cmd::OK);

  ASSERT_EQ(50, balRes.available);
}

TEST_F(UserWithdrawTest, ErrorOnInvalidAddress) {
  cmd::UserWithdrawRequest req;
  cmd::UserWithdrawReply res;
  cmd::UserWithdraw command(session(), nullptr);

  constexpr auto username = "User1";

  createUser(session(), username);

  req.userId = username;
  createBalanceForUsers({1}, 100);
  req.amount = 50;
  req.validateChecksum = false;

  // Last letter is F => [0,-1, *1*]
  req.payoutAddress =
      "WLVZXWARPSYCWJMBZJGXHUOVYBVCEKMNQDMXHCAEJZFLFLMHFYYQQSSLVYWAZWESKXZOROLU"
      "9OQFRVDEF";

  auto status = command.doProcess(&req, &res);
  ASSERT_EQ(status, common::cmd::INVALID_ADDRESS);
}

TEST_F(UserWithdrawTest, ErrorOnInvalidChecksumForPayoutAddress) {
  cmd::UserWithdrawRequest req;
  cmd::UserWithdrawReply res;
  cmd::UserWithdraw command(session(), nullptr);

  constexpr auto username = "User1";

  createUser(session(), username);

  req.userId = username;
  createBalanceForUsers({1}, 100);
  req.amount = 50;
  req.validateChecksum = true;

  // checksum last letter: Y -> Z
  req.payoutAddress =
      "WLVZXWARPSYCWJMBZJGXHUOVYBVCEKMNQDMXHCAEJZFLFLMHFYYQQSSLVYWAZWESKXZOROLU"
      "9OQFRVDEWCHKKXPWGZ";

  auto status = command.doProcess(&req, &res);

  ASSERT_NE(status, common::cmd::OK);
}

};  // namespace
