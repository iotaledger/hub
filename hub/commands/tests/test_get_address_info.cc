// Copyright 2018 IOTA Foundation

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

#include "common/crypto/types.h"
#include "common/stats/session.h"
#include "hub/commands/get_address_info.h"
#include "hub/commands/get_deposit_address.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "runner.h"

using namespace hub;
using namespace sqlpp;
using namespace testing;

namespace {
class GetAddressInfoTest : public CommandTest {};

TEST_F(GetAddressInfoTest, AddressCountInDatabaseShouldChange) {
  rpc::GetAddressInfoRequest req;
  rpc::GetAddressInfoReply res;
  rpc::GetDepositAddressRequest depReq;
  rpc::GetDepositAddressReply depRes;

  rpc::Error err;

  constexpr auto username = "User1";

  createUser(session(), username);

  depReq.set_userid(username);

  cmd::GetDepositAddress depCommand(session());
  cmd::GetAddressInfo command(session());

  ASSERT_TRUE(depCommand.process(&depReq, &depRes).ok());

  req.set_address(
      depRes.address().substr(0, common::crypto::Address::length()));
  ASSERT_TRUE(command.process(&req, &res).ok());
  ASSERT_EQ(res.userid(), username);
}

TEST_F(GetAddressInfoTest, InvalidOrUnknownAddressShouldFail) {
  rpc::GetAddressInfoRequest req;
  rpc::GetAddressInfoReply res;
  rpc::Error err;

  cmd::GetAddressInfo command(session());

  req.set_address("A");
  ASSERT_FALSE(command.process(&req, &res).ok());

  req.set_address(
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999");
  ASSERT_FALSE(command.process(&req, &res).ok());

  req.set_address(
      "992999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999");
  ASSERT_FALSE(command.process(&req, &res).ok());
}

};  // namespace
