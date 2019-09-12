// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "common/stats/session.h"
#include "hub/commands/get_balance.h"
#include "hub/db/db.h"

#include "runner.h"

using namespace hub;
using namespace sqlpp;

namespace {
class GetBalanceTest : public CommandTest {};

TEST_F(GetBalanceTest, UnknownUserShouldFail) {
  rpc::GetBalanceRequest req;
  rpc::GetBalanceReply res;

  req.set_userid("User1");
  cmd::GetBalance command(session());

  auto status = command.process(&req, &res);

  ASSERT_EQ(status, common::cmd::USER_DOES_NOT_EXIST);
}

TEST_F(GetBalanceTest, NewUserHasZeroBalance) {
  rpc::GetBalanceRequest req;
  rpc::GetBalanceReply res;
  rpc::Error err;

  constexpr auto username = "User1";

  createUser(session(), username);

  req.set_userid(username);

  cmd::GetBalance command(session());

  ASSERT_EQ(command.process(&req, &res), common::cmd::OK);

  ASSERT_EQ(0, res.available());
}

};  // namespace
