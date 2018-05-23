// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/user_withdraw.h"
#include "hub/db/db.h"

#include "runner.h"

using namespace hub;
using namespace sqlpp;

namespace {
class UserWithdrawTest : public CommandTest {};

TEST_F(UserWithdrawTest, ErrorOnZeroAmount) {
  rpc::UserWithdrawRequest req;
  rpc::UserWithdrawReply res;
  cmd::UserWithdraw command(session());

  createUser(session(), "a");

  req.set_userid("a");
  req.set_amount(0);
  req.set_payoutaddress(
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999");

  auto status = command.doProcess(&req, &res);

  ASSERT_FALSE(status.ok());
}

};  // namespace
