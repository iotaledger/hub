#include <gtest/gtest.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/get_deposit_address.h"
#include "hub/db/db.h"
#include "hub/stats/session.h"

#include "runner.h"

using namespace hub;
using namespace sqlpp;

namespace {
class GetDepositAddressTest : public CommandTest {};

TEST_F(GetDepositAddressTest, UnknownUserShouldFail) {
  rpc::GetDepositAddressRequest req;
  rpc::GetDepositAddressReply res;

  req.set_userid("User1");
  cmd::GetDepositAddress command(session());

  auto status = command.doProcess(&req, &res);

  ASSERT_FALSE(status.ok());

  auto err = errorFromStatus(status);
  ASSERT_EQ(err.code(), rpc::ErrorCode::USER_DOES_NOT_EXIST);
}

TEST_F(GetDepositAddressTest, AddressCountInDatabaseShouldChange) {
  rpc::GetDepositAddressRequest req;
  rpc::GetDepositAddressReply res;
  rpc::Error err;
  db::sql::UserAddress tbl;

  constexpr auto username = "User1";
  auto& conn = hub::db::DBManager::get().connection();

  createUser(session(), username);

  req.set_userid(username);

  cmd::GetDepositAddress command(session());


  ASSERT_TRUE(command.doProcess(&req, &res).ok());
  std::string address1 = res.address();
  ASSERT_EQ(res.address().length(), 81);
  ASSERT_TRUE(command.doProcess(&req, &res).ok());
  ASSERT_EQ(res.address().length(), 81);
  ASSERT_NE(address1, res.address());

  ASSERT_EQ(2, conn(select(count(tbl.id)).from(tbl).unconditionally())
            .front()
            .count);

}

TEST_F(GetDepositAddressTest, AddressShouldHaveCorrectLength) {
  rpc::GetDepositAddressRequest req;
  rpc::GetDepositAddressReply res;
  rpc::Error err;

  constexpr auto username = "User1";

  createUser(session(), username);

  req.set_userid(username);

  cmd::GetDepositAddress command(session());

  ASSERT_TRUE(command.doProcess(&req, &res).ok());
  ASSERT_EQ(res.address().length(), 81);
}

};  // namespace
