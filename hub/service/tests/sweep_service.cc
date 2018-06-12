// Copyright 2018 IOTA Foundation

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "cppclient/api.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/service/sweep_service.h"
#include "hub/tests/runner.h"

using namespace testing;

using namespace hub;
using namespace hub::service;
using namespace cppclient;

namespace {

class SweepServiceTest : public hub::Test {};

class MockMonitor : public SweepService {
 public:
  using SweepService::doTick;
  using SweepService::SweepService;

  const std::string name() const override { return "MockSweepService"; }
};

class MockAPI : public cppclient::IotaAPI {
 public:
  virtual ~MockAPI() {}

  MOCK_METHOD0(isNodeSolid, bool());

  MOCK_METHOD1(getBalances, std::unordered_map<std::string, uint64_t>(
                                const std::vector<std::string>& addresses));

  MOCK_METHOD1(getConfirmedBundlesForAddress,
               std::vector<Bundle>(const std::string& address));

  MOCK_METHOD1(
      filterConfirmedTails,
      std::unordered_set<std::string>(const std::vector<std::string>& tails));
};

TEST_F(SweepServiceTest, DoSomething) {}

}  // namespace
