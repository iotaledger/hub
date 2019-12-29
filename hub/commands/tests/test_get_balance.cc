// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include "hub/commands/get_balance.h"

#include "runner.h"

using namespace hub;
using namespace sqlpp;

namespace {
class GetBalanceTest : public CommandTest {};

TEST_F(GetBalanceTest, UnknownUserShouldFail) {
  cmd::GetBalanceRequest req = {.userId = "User1"};
  cmd::GetBalanceReply res;

  cmd::GetBalance command(session());

  auto status = command.process(&req, &res);

  ASSERT_EQ(status, common::cmd::USER_DOES_NOT_EXIST);
}

TEST_F(GetBalanceTest, NewUserHasZeroBalance) {
  constexpr auto username = "User1";
  cmd::GetBalanceRequest req = {.userId = username};
  cmd::GetBalanceReply res;

  createUser(session(), username);

  cmd::GetBalance command(session());

  ASSERT_EQ(command.process(&req, &res), common::cmd::OK);

  ASSERT_EQ(0, res.available);
}

};  // namespace
