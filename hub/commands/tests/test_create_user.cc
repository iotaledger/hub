// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include "hub/commands/create_user.h"
#include "runner.h"

using namespace hub;
using namespace sqlpp;

namespace {
class CreateUserTest : public CommandTest {};

TEST_F(CreateUserTest, ErrorOnDuplicate) {
  createUser(session(), "User1");
  auto status = createUser(session(), "User1");

  ASSERT_EQ(status, common::cmd::USER_EXISTS);
}

TEST_F(CreateUserTest, CreateUsers) {
  ASSERT_EQ(createUser(session(), "User1"), common::cmd::OK);
  ASSERT_EQ(createUser(session(), "User2"), common::cmd::OK);
}

};  // namespace
