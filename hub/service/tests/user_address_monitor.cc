// Copyright 2018 IOTA Foundation

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "cppclient/api.h"
#include "hub/service/user_address_monitor.h"
#include "hub/tests/runner.h"

using namespace testing;

using namespace hub;
using namespace hub::service;
using namespace cppclient;
namespace {

class UserAddressMonitorTest : public hub::Test {};

class MockMonitor : public UserAddressMonitor {
 public:
  using UserAddressMonitor::doTick;
  using UserAddressMonitor::UserAddressMonitor;

  MOCK_METHOD0(monitoredAddresses,
               std::vector<std::tuple<uint64_t, std::string>>(void));

  const std::string name() const override { return "MockMonitor"; }
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

}  // namespace
