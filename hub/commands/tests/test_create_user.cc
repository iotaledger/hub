// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/create_user.h"
#include "hub/db/db.h"

#include "runner.h"

using namespace hub;
using namespace sqlpp;

namespace {
class CreateUserTest : public CommandTest {};

TEST_F(CreateUserTest, ErrorOnDuplicate) {

  createUser(session(), "User1");
  auto status = createUser(session(), "User1");
  auto err = errorFromStatus(status);

  ASSERT_EQ(grpc::StatusCode::FAILED_PRECONDITION, status.error_code());
  ASSERT_EQ(err.code(), rpc::ErrorCode::USER_EXISTS);

}

TEST_F(CreateUserTest, CreateUsers) {

  ASSERT_TRUE(createUser(session(), "User1").ok());
  ASSERT_TRUE(createUser(session(), "User2").ok());
}

};  // namespace
