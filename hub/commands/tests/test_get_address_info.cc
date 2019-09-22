// Copyright 2018 IOTA Foundation

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "common/crypto/types.h"
#include "hub/commands/get_address_info.h"
#include "hub/commands/get_deposit_address.h"
#include "hub/db/helper.h"

#include "runner.h"

using namespace hub;
using namespace sqlpp;
using namespace testing;

namespace {
class GetAddressInfoTest : public CommandTest {};

TEST_F(GetAddressInfoTest, AddressCountInDatabaseShouldChange) {
  cmd::GetAddressInfoRequest req;
  cmd::GetAddressInfoReply res;
  cmd::GetDepositAddressRequest depReq;
  cmd::GetDepositAddressReply depRes;

  constexpr auto username = "User1";

  createUser(session(), username);

  depReq.userId = username;

  cmd::GetDepositAddress depCommand(session());
  cmd::GetAddressInfo command(session());

  ASSERT_EQ(depCommand.process(&depReq, &depRes), common::cmd::OK);

  req.address = depRes.address.substr(0, common::crypto::Address::length());
  ASSERT_EQ(command.process(&req, &res), common::cmd::OK);
  ASSERT_EQ(res.userId, username);
}

TEST_F(GetAddressInfoTest, InvalidOrUnknownAddressShouldFail) {
  cmd::GetAddressInfoRequest req;
  cmd::GetAddressInfoReply res;

  cmd::GetAddressInfo command(session());

  req.address = "A";
  ASSERT_NE(command.process(&req, &res), common::cmd::OK);

  req.address =
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999";
  ASSERT_NE(command.process(&req, &res), common::cmd::OK);
  req.address =
      "992999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999";
  ASSERT_NE(command.process(&req, &res), common::cmd::OK);
}

};  // namespace
