// Copyright 2018 IOTA Foundation

#ifndef HUB_TESTS_RUNNER_H_
#define HUB_TESTS_RUNNER_H_

#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "common/crypto/argon2_provider.h"
#include "common/crypto/manager.h"
#include "common/crypto/types.h"
#include "common/stats/session.h"
#include "hub/db/db.h"

namespace hub {
class Test : public ::testing::Test {
 public:
  virtual void SetUp() {
    common::crypto::CryptoManager::get().setProvider(
        std::make_unique<common::crypto::Argon2Provider>("abcdefghij"));

    db::Config config;

    using std::string_literals::operator""s;

    config.type = "sqlite3";
    config.database =
        "file:"s + common::crypto::UUID().str() + "?mode=memory&cache=shared"s;

    std::replace(config.database.begin(), config.database.end(), '+', 'X');
    std::replace(config.database.begin(), config.database.end(), '/', 'Y');

    auto& db = hub::db::DBManager::get();
    db.setConnectionConfig(config);
    db.loadSchema();

    _session = std::make_shared<common::ClientSession>();
  }

  virtual void TearDown() {
    auto& db = hub::db::DBManager::get();
    db.resetConnection();
    _session = nullptr;
  }

  std::shared_ptr<common::ClientSession> session() { return _session; }

 private:
  std::shared_ptr<common::ClientSession> _session;
};

}  //  namespace hub
#endif  // HUB_TESTS_RUNNER_H_
