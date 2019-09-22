// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include "hub/commands/get_user_history.h"
#include "runner.h"

using namespace hub;
using namespace sqlpp;

namespace {
class GetUserHistoryTest : public CommandTest {};

TEST_F(GetUserHistoryTest, UnknownUserShouldFail) {
  hub::cmd::GetUserHistoryRequest req;
  hub::cmd::GetUserHistoryReply res;

  req.userId = "User1";
  cmd::GetUserHistory command(session());

  auto status = command.process(&req, &res);

  ASSERT_FALSE(status == common::cmd::OK);

  ASSERT_EQ(status, common::cmd::USER_DOES_NOT_EXIST);
}

TEST_F(GetUserHistoryTest, NewUserHasNoHistory) {
  hub::cmd::GetUserHistoryRequest req;
  hub::cmd::GetUserHistoryReply res;

  constexpr auto username = "User1";

  createUser(session(), username);

  req.userId = username;

  cmd::GetUserHistory command(session());

  ASSERT_EQ(command.process(&req, &res), common::cmd::OK);

  ASSERT_EQ(0, res.events.size());
}

};  // namespace
