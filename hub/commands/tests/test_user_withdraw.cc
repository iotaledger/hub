// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/get_balance.h"
#include "hub/commands/tests/helper.h"
#include "hub/commands/user_withdraw.h"
#include "hub/db/db.h"

#include "runner.h"

using namespace hub;
using namespace hub::tests;
using namespace sqlpp;

namespace {
class UserWithdrawTest : public CommandTest {};

TEST_F(UserWithdrawTest, ErrorOnInvalidPayoutAddress) {
  rpc::UserWithdrawRequest req;
  rpc::UserWithdrawReply res;
  cmd::UserWithdraw command(session(), nullptr);

  createUser(session(), "a");

  req.set_userid("a");
  req.set_amount(0);

  req.set_payoutaddress("999999999");
  ASSERT_FALSE(command.doProcess(&req, &res).ok());
}

TEST_F(UserWithdrawTest, ErrorOnZeroAmount) {
  rpc::UserWithdrawRequest req;
  rpc::UserWithdrawReply res;
  cmd::UserWithdraw command(session(), nullptr);

  createUser(session(), "a");

  req.set_userid("a");
  req.set_amount(0);
  req.set_payoutaddress(
      "WLVZXWARPSYCWJMBZJGXHUOVYBVCEKMNQDMXHCAEJZFLFLMHFYYQQSSLVYWAZWESKXZOROLU"
      "9OQFRVDEWCHKKXPWGY");

  auto status = command.process(&req, &res);

  ASSERT_FALSE(status.ok());
}

TEST_F(UserWithdrawTest, WithdrawalUpdatesUserBalance) {
  rpc::UserWithdrawRequest req;
  rpc::UserWithdrawReply res;
  cmd::UserWithdraw command(session(), nullptr);

  constexpr auto username = "User1";

  createUser(session(), username);

  req.set_userid(username);
  createBalanceForUsers({1}, 100);
  req.set_amount(50);
  req.set_validatechecksum(true);
  req.set_payoutaddress(
      "WLVZXWARPSYCWJMBZJGXHUOVYBVCEKMNQDMXHCAEJZFLFLMHFYYQQSSLVYWAZWESKXZOROLU"
      "9OQFRVDEWCHKKXPWGY");

  auto status = command.doProcess(&req, &res);

  ASSERT_TRUE(status.ok());

  rpc::GetBalanceRequest balReq;
  rpc::GetBalanceReply balRes;
  rpc::Error err;

  balReq.set_userid(username);
  cmd::GetBalance balCommand(session());

  ASSERT_TRUE(balCommand.doProcess(&balReq, &balRes).ok());

  ASSERT_EQ(50, balRes.available());
}

TEST_F(UserWithdrawTest, ErrorOnInvalidAddress) {
  rpc::UserWithdrawRequest req;
  rpc::UserWithdrawReply res;
  cmd::UserWithdraw command(session(), nullptr);

  constexpr auto username = "User1";

  createUser(session(), username);

  req.set_userid(username);
  createBalanceForUsers({1}, 100);
  req.set_amount(50);
  req.set_validatechecksum(false);

  // Last letter is F => [0,-1, *1*]
  req.set_payoutaddress(
      "WLVZXWARPSYCWJMBZJGXHUOVYBVCEKMNQDMXHCAEJZFLFLMHFYYQQSSLVYWAZWESKXZOROLU"
      "9OQFRVDEF");

  auto status = command.doProcess(&req, &res);
  ASSERT_FALSE(status.ok());

  ASSERT_EQ(hub::rpc::ErrorCode::INELIGIBLE_ADDRESS,
            errorCodeFromDetails(status.error_details()));
}

TEST_F(UserWithdrawTest, ErrorOnInvalidChecksumForPayoutAddress) {
  rpc::UserWithdrawRequest req;
  rpc::UserWithdrawReply res;
  cmd::UserWithdraw command(session(), nullptr);

  constexpr auto username = "User1";

  createUser(session(), username);

  req.set_userid(username);
  createBalanceForUsers({1}, 100);
  req.set_amount(50);
  req.set_validatechecksum(true);

  // checksum last letter: Y -> Z
  req.set_payoutaddress(
      "WLVZXWARPSYCWJMBZJGXHUOVYBVCEKMNQDMXHCAEJZFLFLMHFYYQQSSLVYWAZWESKXZOROLU"
      "9OQFRVDEWCHKKXPWGZ");

  auto status = command.doProcess(&req, &res);

  ASSERT_FALSE(status.ok());
}

};  // namespace
