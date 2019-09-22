// Copyright 2018 IOTA Foundation

#include <chrono>
#include <thread>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "hub/commands/balance_subscription.h"
#include "runner.h"

using namespace testing;
using namespace hub;
using namespace hub::cmd;

namespace {

class BalanceSubscriptionTest : public CommandTest {};

class MockBalanceSubscription : public BalanceSubscription {
 public:
  using BalanceSubscription::BalanceSubscription;
  using BalanceSubscription::process;
  MOCK_METHOD1(getAllUsersAccountBalancesSinceTimePoint,
               std::vector<db::UserAccountBalanceEvent>(
                   std::chrono::system_clock::time_point lastCheck));
  MOCK_METHOD1(getAllUserAddressesBalancesSinceTimePoint,
               std::vector<db::UserAddressBalanceEvent>(
                   std::chrono::system_clock::time_point lastCheck));
  MOCK_METHOD1(getAllHubAddressesBalancesSinceTimePoint,
               std::vector<db::HubAddressBalanceEvent>(
                   std::chrono::system_clock::time_point lastCheck));
};

TEST_F(BalanceSubscriptionTest, BalanceSubscriptionWriteAllEvents) {
  auto clientSession = std::make_shared<common::ClientSession>();
  MockBalanceSubscription mockCmd(clientSession);
  std::vector<cmd::BalanceEvent> events;

  hub::cmd::BalanceSubscriptionRequest request;

  std::vector<db::UserAccountBalanceEvent> userAccountEvents;
  userAccountEvents.push_back(db::UserAccountBalanceEvent{});
  userAccountEvents.push_back(db::UserAccountBalanceEvent{});
  userAccountEvents.push_back(db::UserAccountBalanceEvent{});
  userAccountEvents.push_back(db::UserAccountBalanceEvent{});

  std::vector<db::UserAddressBalanceEvent> userAddressEvents;
  userAddressEvents.push_back(db::UserAddressBalanceEvent{});
  userAddressEvents.push_back(db::UserAddressBalanceEvent{});

  std::vector<db::HubAddressBalanceEvent> hubAddressEvents;
  hubAddressEvents.push_back(db::HubAddressBalanceEvent{});
  hubAddressEvents.push_back(db::HubAddressBalanceEvent{});

  EXPECT_CALL(mockCmd, getAllUsersAccountBalancesSinceTimePoint(_))
      .Times(1)
      .WillOnce(Return(userAccountEvents));
  EXPECT_CALL(mockCmd, getAllUserAddressesBalancesSinceTimePoint(_))
      .Times(1)
      .WillOnce(Return(userAddressEvents));
  EXPECT_CALL(mockCmd, getAllHubAddressesBalancesSinceTimePoint(_))
      .Times(1)
      .WillOnce(Return(hubAddressEvents));
  ASSERT_EQ(mockCmd.process(&request, &events), common::cmd::OK);
}
};  // namespace
