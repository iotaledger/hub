#include <gtest/gtest.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/get_balance.h"
#include "hub/db/db.h"
#include "hub/stats/session.h"

#include "runner.h"

using namespace iota;
using namespace sqlpp;

namespace {
class GetBalanceTest : public CommandTest {};

TEST_F(GetBalanceTest, UnknownUserShouldFail) {
  rpc::GetBalanceRequest req;
  rpc::GetBalanceReply res;

  req.set_userid("User1");
  cmd::GetBalance command(session());

  auto status = command.doProcess(&req, &res);

  EXPECT_FALSE(status.ok());

  auto err = errorFromStatus(status);
  EXPECT_EQ(err.code(), rpc::ErrorCode::USER_DOES_NOT_EXIST);
}

TEST_F(GetBalanceTest, NewUserHasZeroBalance) {
  rpc::GetBalanceRequest req;
  rpc::GetBalanceReply res;
  rpc::Error err;

  constexpr auto username = "User1";

  createUser(session(), username);

  req.set_userid("User1");

  cmd::GetBalance command(session());

  EXPECT_TRUE(command.doProcess(&req, &res).ok());

  EXPECT_EQ(0, res.available());
}

};  // namespace
