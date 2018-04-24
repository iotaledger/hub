#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "hub/iota/api.h"
#include "hub/service/user_address_monitor.h"
#include "hub/tests/runner.h"

using namespace testing;

using namespace hub;
using namespace hub::iota;
using namespace hub::service;

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

class MockAPI : public iota::IotaAPI {
 public:
  virtual ~MockAPI() {}

  MOCK_METHOD1(getBalances, std::unordered_map<std::string, uint64_t>(
                                const std::vector<std::string>& addresses));

  MOCK_METHOD1(getConfirmedBundlesForAddress,
               std::vector<Bundle>(const std::string& address));

  MOCK_METHOD1(
      filterConfirmedTails,
      std::unordered_set<std::string>(const std::vector<std::string>& tails));
};

TEST_F(UserAddressMonitorTest, OnStartShouldInitialise) {
  auto sapi = std::make_shared<MockAPI>();
  auto& api = *sapi;
  MockMonitor monitor(sapi, std::chrono::milliseconds(1));

  std::vector<AddressMonitor::BalanceChange> changes = {
      {1, "ABC", 0, -1000},
      {2, "ADDRESS", 10000, 5000},
      {1, "BCD", 1000, 1000}};

  monitor.onBalancesChanged(changes);

  EXPECT_EQ(1, 0);
}

}  // namespace
