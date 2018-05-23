// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>
#include "proto/hub.pb.h"
#include "schema/schema.h"
#include "hub/commands/get_user_history.h"
#include "hub/db/db.h"
#include "hub/stats/session.h"
#include "runner.h"

using namespace hub;
using namespace sqlpp;

namespace {
class GetUserHistoryTest : public CommandTest {};

TEST_F(GetUserHistoryTest, UnknownUserShouldFail) {
  rpc::GetUserHistoryRequest req;
  rpc::GetUserHistoryReply res;

  req.set_userid("User1");
  cmd::GetUserHistory command(session());

  auto status = command.process(&req, &res);

  ASSERT_FALSE(status.ok());

  auto err = errorFromStatus(status);
  ASSERT_EQ(err.code(), rpc::ErrorCode::USER_DOES_NOT_EXIST);
}

TEST_F(GetUserHistoryTest, NewUserHasNoHistory) {
  rpc::GetUserHistoryRequest req;
  rpc::GetUserHistoryReply res;
  rpc::Error err;

  constexpr auto username = "User1";

  createUser(session(), username);

  req.set_userid(username);

  cmd::GetUserHistory command(session());

  ASSERT_TRUE(command.process(&req, &res).ok());

  ASSERT_EQ(0, res.events_size());
}

};  // namespace
