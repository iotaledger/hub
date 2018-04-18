#include <gtest/gtest.h>
#include <sqlpp11/select.h>
#include <sqlpp11/functions.h>

#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/create_user.h"
#include "hub/db/db.h"
#include "hub/stats/session.h"

using namespace iota;
using namespace sqlpp;

namespace {
class CommandTest : public ::testing::Test {
 public:
  virtual void SetUp() {
    auto db = iota::db::DBManager::get();
    db.resetConnection();
    db.loadSchema(true);
  }
};

TEST_F(CommandTest, ErrorOnDuplicate) {
  rpc::CreateUserRequest req;
  rpc::CreateUserReply res;
  rpc::Error err;
  db::sql::UserAccount tbl;

  auto session = std::make_shared<ClientSession>();
  auto& conn = iota::db::DBManager::get().connection();

  req.set_identifier("User1");

  cmd::CreateUser command(session);
  command.doProcess(&req, &res);
  auto status = command.doProcess(&req, &res);
  err.ParseFromString(status.error_details());

  EXPECT_EQ(grpc::StatusCode::FAILED_PRECONDITION, status.error_code());
  EXPECT_EQ(err.code(), rpc::ErrorCode::USER_EXISTS);
  EXPECT_EQ(1, conn(select(count(tbl.identifier)).from(tbl).unconditionally())
                   .front()
                   .count);
}

TEST_F(CommandTest, CreateUsers) {
  rpc::CreateUserRequest req;
  rpc::CreateUserReply res;
  db::sql::UserAccount tbl;

  auto session = std::make_shared<ClientSession>();
  auto& conn = iota::db::DBManager::get().connection();

  req.set_identifier("User1");

  {
    cmd::CreateUser command(session);
    command.doProcess(&req, &res);
  }

  req.set_identifier("User2");
  {
    cmd::CreateUser command(session);
    command.doProcess(&req, &res);
  }

  EXPECT_EQ(2, conn(select(count(tbl.identifier)).from(tbl).unconditionally())
                   .front()
                   .count);
}

};  // namespace
