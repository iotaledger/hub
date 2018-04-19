#include <gtest/gtest.h>

#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/create_user.h"
#include "hub/db/db.h"

#include "runner.h"

using namespace iota;
using namespace sqlpp;

namespace {
class CreateUserTest : public CommandTest {};

TEST_F(CreateUserTest, ErrorOnDuplicate) {
  db::sql::UserAccount tbl;

  auto& conn = iota::db::DBManager::get().connection();

  createUser(session(), "User1");
  auto status = createUser(session(), "User1");
  auto err = errorFromStatus(status);

  EXPECT_EQ(grpc::StatusCode::FAILED_PRECONDITION, status.error_code());
  EXPECT_EQ(err.code(), rpc::ErrorCode::USER_EXISTS);
  EXPECT_EQ(1, conn(select(count(tbl.identifier)).from(tbl).unconditionally())
                   .front()
                   .count);
}

TEST_F(CreateUserTest, CreateUsers) {
  db::sql::UserAccount tbl;

  auto& conn = iota::db::DBManager::get().connection();

  EXPECT_TRUE(createUser(session(), "User1").ok());
  EXPECT_TRUE(createUser(session(), "User2").ok());

  EXPECT_EQ(2, conn(select(count(tbl.identifier)).from(tbl).unconditionally())
                   .front()
                   .count);
}

};  // namespace
