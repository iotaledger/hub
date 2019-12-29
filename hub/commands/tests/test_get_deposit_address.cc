// Copyright 2018 IOTA Foundation

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "common/crypto/types.h"
#include "hub/commands/get_deposit_address.h"
#include "hub/db/helper.h"

#include "runner.h"

using namespace hub;
using namespace sqlpp;
using namespace testing;

namespace {
class GetDepositAddressTest : public CommandTest {};

TEST_F(GetDepositAddressTest, UnknownUserShouldFail) {
  hub::cmd::GetDepositAddressRequest req;
  hub::cmd::GetDepositAddressReply res;

  req.userId = "User1";
  cmd::GetDepositAddress command(session());

  auto status = command.process(&req, &res);

  ASSERT_EQ(status, common::cmd::USER_DOES_NOT_EXIST);
}

TEST_F(GetDepositAddressTest, AddressCountInDatabaseShouldChange) {
  cmd::GetDepositAddressRequest req;
  cmd::GetDepositAddressReply res;

  constexpr auto username = "User1";
  auto& conn = hub::db::DBManager::get().connection();

  createUser(session(), username);

  req.userId = username;
  req.includeChecksum = false;

  cmd::GetDepositAddress command(session());

  ASSERT_EQ(command.process(&req, &res), common::cmd::OK);
  std::string address1 = res.address;
  ASSERT_EQ(res.address.length(), common::crypto::Address::length());
  ASSERT_EQ(command.process(&req, &res), common::cmd::OK);
  ASSERT_EQ(res.address.length(), common::crypto::Address::length());
  ASSERT_NE(address1, res.address);

  /*ASSERT_EQ(
    2, conn(select(count(tbl.id)).from(tbl).unconditionally()).front().count);*/

  auto unswept = conn.unsweptUserAddresses();

  ASSERT_EQ(2, unswept.size());
  ASSERT_NE(std::find_if(unswept.begin(), unswept.end(),
                         [&address1](auto& ref) {
                           return std::get<1>(ref) == address1;
                         }),
            unswept.end());

  ASSERT_NE(std::find_if(
                unswept.begin(), unswept.end(),
                [&res](auto& ref) { return std::get<1>(ref) == res.address; }),
            unswept.end());
}

TEST_F(GetDepositAddressTest, AddressShouldHaveCorrectLength) {
  cmd::GetDepositAddressRequest req;
  cmd::GetDepositAddressReply res;

  constexpr auto username = "User1";

  createUser(session(), username);

  req.userId = username;
  req.includeChecksum = true;

  cmd::GetDepositAddress command(session());

  ASSERT_EQ(command.process(&req, &res), common::cmd::OK);
  ASSERT_EQ(res.address.length(), common::crypto::Address::length() +
                                      common::crypto::Checksum::length());

  req.includeChecksum = false;
  ASSERT_EQ(command.process(&req, &res), common::cmd::OK);
  ASSERT_EQ(res.address.length(), common::crypto::Address::length());
}

};  // namespace
