// Copyright 2018 IOTA Foundation

#ifndef HUB_TESTS_RUNNER_H_
#define HUB_TESTS_RUNNER_H_

#include <memory>

#include <gtest/gtest.h>

#include "hub/crypto/local_provider.h"
#include "hub/crypto/manager.h"
#include "hub/db/db.h"
#include "hub/stats/session.h"

namespace hub {
class Test : public ::testing::Test {
 public:
  virtual void SetUp() {
    hub::crypto::CryptoManager::get().setProvider(
        std::make_unique<crypto::LocalProvider>("abcdefghij"));

    db::Config config;

    config.type = "sqlite3";
    config.host = ":memory:";

    auto db = hub::db::DBManager::get();
    db.setConnectionConfig(config);
    db.loadSchema(true);

    _session = std::make_shared<ClientSession>();
  }

  virtual void TearDown() {
    auto db = hub::db::DBManager::get();
    db.resetConnection();
    _session = nullptr;
  }

  std::shared_ptr<ClientSession> session() { return _session; }

 private:
  std::shared_ptr<ClientSession> _session;
};

}  //  namespace hub
#endif  // HUB_TESTS_RUNNER_H_
