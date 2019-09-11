// Copyright 2018 IOTA Foundation

#ifndef HUB_COMMANDS_TESTS_RUNNER_H_
#define HUB_COMMANDS_TESTS_RUNNER_H_

#include <gtest/gtest.h>

#include "common/crypto/manager.h"
#include "common/stats/session.h"
#include "hub/commands/create_user.h"
#include "hub/db/db.h"
#include "hub/tests/runner.h"
#include "proto/hub.pb.h"

namespace hub {
class CommandTest : public hub::Test {
 public:
  grpc::Status createUser(std::shared_ptr<common::ClientSession> session,
                          std::string username) {
    cmd::CreateUserRequest req;
    cmd::CreateUserReply res;

    req.userId = std::move(username);
    cmd::CreateUser command(std::move(session));
    return command.process(&req, &res);
  }

  rpc::Error errorFromStatus(grpc::Status& status) {
    rpc::Error err;
    err.ParseFromString(status.error_details());

    return err;
  }
};

}  // namespace hub
#endif  // HUB_COMMANDS_TESTS_RUNNER_H_
