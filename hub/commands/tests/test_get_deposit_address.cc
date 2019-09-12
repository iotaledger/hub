// Copyright 2018 IOTA Foundation

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

#include "common/crypto/types.h"
#include "common/stats/session.h"
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
class GetDepositAddressTest : public CommandTest {};

TEST_F(GetDepositAddressTest, UnknownUserShouldFail) {
  rpc::GetDepositAddressRequest req;
  rpc::GetDepositAddressReply res;

  req.set_userid("User1");
  cmd::GetDepositAddress command(session());

  auto status = command.process(&req, &res);

  ASSERT_EQ(status, common::cmd::USER_DOES_NOT_EXIST);
}

TEST_F(GetDepositAddressTest, AddressCountInDatabaseShouldChange) {
  rpc::GetDepositAddressRequest req;
  rpc::GetDepositAddressReply res;
  rpc::Error err;

  constexpr auto username = "User1";
  auto& conn = hub::db::DBManager::get().connection();

  createUser(session(), username);

  req.set_userid(username);

  cmd::GetDepositAddress command(session());

  ASSERT_EQ(command.process(&req, &res), common::cmd::OK);
  std::string address1 = res.address();
  ASSERT_EQ(res.address().length(), common::crypto::Address::length());
  ASSERT_EQ(command.process(&req, &res), common::cmd::OK);
  ASSERT_EQ(res.address().length(), common::crypto::Address::length());
  ASSERT_NE(address1, res.address());

  /*ASSERT_EQ(
    2, conn(select(count(tbl.id)).from(tbl).unconditionally()).front().count);*/

  auto unswept = conn.unsweptUserAddresses();

  ASSERT_EQ(2, unswept.size());
  ASSERT_NE(std::find_if(unswept.begin(), unswept.end(),
                         [&address1](auto& ref) {
                           return std::get<1>(ref) == address1;
                         }),
            unswept.end());

  ASSERT_NE(std::find_if(unswept.begin(), unswept.end(),
                         [&res](auto& ref) {
                           return std::get<1>(ref) == res.address();
                         }),
            unswept.end());
}

TEST_F(GetDepositAddressTest, AddressShouldHaveCorrectLength) {
  rpc::GetDepositAddressRequest req;
  rpc::GetDepositAddressReply res;
  rpc::Error err;

  constexpr auto username = "User1";

  createUser(session(), username);

  req.set_userid(username);
  req.set_includechecksum(true);

  cmd::GetDepositAddress command(session());

  ASSERT_EQ(command.process(&req, &res), common::cmd::OK);
  ASSERT_EQ(res.address().length(), common::crypto::Address::length() +
                                        common::crypto::Checksum::length());

  req.set_includechecksum(false);
  ASSERT_EQ(command.process(&req, &res), common::cmd::OK);
  ASSERT_EQ(res.address().length(), common::crypto::Address::length());
}

};  // namespace
