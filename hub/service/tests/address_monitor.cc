// Copyright 2018 IOTA Foundation

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "hub/iota/api.h"
#include "hub/service/address_monitor.h"

using namespace testing;

using namespace hub;
using namespace hub::iota;
using namespace hub::service;

namespace {

class AddressMonitorTest : public ::testing::Test {};

class MockMonitor : public AddressMonitor {
 public:
  using AddressMonitor::AddressMonitor;
  using AddressMonitor::doTick;

  MOCK_METHOD0(monitoredAddresses,
               std::vector<std::tuple<uint64_t, std::string>>(void));
  MOCK_METHOD1(onBalancesChanged,
               bool(const std::vector<BalanceChange>& changed));

  const std::string name() const override { return "MockMonitor"; }
};

class MockAPI : public iota::IotaAPI {
 public:
  virtual ~MockAPI() {}

  MOCK_METHOD0(isNodeSolid, bool());

  MOCK_METHOD1(getBalances, std::unordered_map<std::string, uint64_t>(
                                const std::vector<std::string>& addresses));

  MOCK_METHOD1(getConfirmedBundlesForAddress,
               std::vector<Bundle>(const std::string& address));

  MOCK_METHOD2(filterConfirmedTails,
               std::unordered_set<std::string>(
                   const std::vector<std::string>& tails,
                   const std::optional<std::string>& reference));

  MOCK_METHOD2(findTransactions,
               std::vector<std::string>(
                   std::optional<std::vector<std::string>> addresses,
                   std::optional<std::vector<std::string>> bundles));

  MOCK_METHOD0(getNodeInfo, NodeInfo());

  MOCK_METHOD1(getTrytes,
               std::vector<Transaction>(const std::vector<std::string>&));
  MOCK_METHOD1(filterConsistentTails, std::unordered_set<std::string>(
                                          const std::vector<std::string>&));
};

TEST_F(AddressMonitorTest, OnStartShouldInitialise) {
  auto sapi = std::make_shared<MockAPI>();
  auto& api = *sapi;
  MockMonitor monitor(sapi, std::chrono::milliseconds(1));

  EXPECT_CALL(monitor, monitoredAddresses()).Times(1);
  EXPECT_CALL(monitor, onBalancesChanged(_)).Times(0);
  EXPECT_CALL(api, getBalances(_)).Times(0);
  monitor.onStart();
}

TEST_F(AddressMonitorTest, Tick) {
  auto sapi = std::make_shared<MockAPI>();
  auto& api = *sapi;
  MockMonitor monitor(sapi, std::chrono::milliseconds(1));

  auto address =
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAA";

  std::vector<std::tuple<uint64_t, std::string>> monitored = {
      std::make_tuple(0, address)};

  EXPECT_CALL(monitor, monitoredAddresses()).Times(1);
  EXPECT_CALL(monitor, onBalancesChanged(_)).Times(0);
  EXPECT_CALL(api, getBalances(_)).Times(0);
  monitor.onStart();

  EXPECT_CALL(monitor, monitoredAddresses())
      .Times(5)
      .WillRepeatedly(Return(monitored));

  EXPECT_CALL(api, getBalances(std::vector<std::string>{address}))
      .Times(1)
      .WillOnce(
          Return(std::unordered_map<std::string, uint64_t>{{address, 0}}));

  monitor.doTick();

  EXPECT_CALL(api, getBalances(std::vector<std::string>{address}))
      .Times(1)
      .WillOnce(
          Return(std::unordered_map<std::string, uint64_t>{{address, 1000}}));

  // Test new balance arrival
  std::vector<AddressMonitor::BalanceChange> ex1 = {{0, address, 1000, 1000}};
  EXPECT_CALL(monitor,
              onBalancesChanged(ElementsAreArray(ex1.cbegin(), ex1.cend())))
      .Times(1)
      .WillOnce(Return(true));

  monitor.doTick();

  EXPECT_CALL(api, getBalances(std::vector<std::string>{address}))
      .Times(3)
      .WillRepeatedly(
          Return(std::unordered_map<std::string, uint64_t>{{address, 0}}));

  // Update is refused
  std::vector<AddressMonitor::BalanceChange> ex2 = {{0, address, 0, -1000}};
  EXPECT_CALL(monitor,
              onBalancesChanged(ElementsAreArray(ex2.cbegin(), ex2.cend())))
      .Times(1)
      .WillOnce(Return(false));

  monitor.doTick();

  // Update is accepted
  EXPECT_CALL(monitor,
              onBalancesChanged(ElementsAreArray(ex2.cbegin(), ex2.cend())))
      .Times(1)
      .WillOnce(Return(true));

  monitor.doTick();

  // No change
  EXPECT_CALL(monitor, onBalancesChanged(_)).Times(0);

  monitor.doTick();
}

TEST_F(AddressMonitorTest, RemoveUnmonitoredAddresses) {
  auto sapi = std::make_shared<MockAPI>();
  auto& api = *sapi;
  MockMonitor monitor(sapi, std::chrono::milliseconds(1));

  auto addressA =
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAA";

  auto addressB =
      "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
      "BBBBBBBBB";

  auto addressC =
      "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
      "CCCCCCCCC";

  // std::unordered_map<uint64_t, uint64_t>
  std::vector<std::tuple<uint64_t, std::string>> monitored = {
      std::make_tuple(0, addressA), std::make_tuple(1, addressB),
      std::make_tuple(2, addressC)};

  EXPECT_CALL(monitor, monitoredAddresses())
      .Times(1)
      .WillOnce(Return(monitored));

  EXPECT_CALL(api, getBalances(_))
      .Times(1)
      .WillOnce(Return(std::unordered_map<std::string, uint64_t>{
          {addressA, 10}, {addressB, 20}, {addressC, 30}}));

  monitor.onStart();

  std::unordered_map<uint64_t, uint64_t> balances = {{0, 10}, {1, 20}, {2, 30}};

  EXPECT_EQ(balances, monitor.balances());

  monitored.pop_back();  // loosing addressC

  EXPECT_CALL(monitor, monitoredAddresses())
      .Times(1)
      .WillOnce(Return(monitored));
  EXPECT_CALL(api, getBalances(_))
      .Times(1)
      .WillOnce(Return(std::unordered_map<std::string, uint64_t>{
          {addressA, 10}, {addressB, 20}}));

  std::unordered_map<uint64_t, uint64_t> balancesAfter = {{0, 10}, {1, 20}};

  monitor.doTick();

  EXPECT_EQ(balancesAfter, monitor.balances());
}

}  // namespace
