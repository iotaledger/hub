// Copyright 2018 IOTA Foundation

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <chrono>
#include <thread>

#include "hub/commands/balance_subscription.h"
#include "hub/stats/session.h"
#include "proto/hub.grpc.pb.h"
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
  using IWriter = grpc::ServerWriterInterface<rpc::BalanceEvent>;
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

class MockServerWriter : public grpc::ServerWriterInterface<rpc::BalanceEvent> {
 public:
  MOCK_METHOD0(SendInitialMetadata, void(void));
  MOCK_METHOD2(Write,
               bool(const hub::rpc::BalanceEvent& msg, grpc::WriteOptions));
  // bool 	Write (const W &msg) - Is not virtual, but the method above is,
  // and will be called internally when the non virtual version is called
};

TEST_F(BalanceSubscriptionTest, BalanceSubscriptionWriteAllEvents) {
  auto clientSession = std::make_shared<ClientSession>();
  MockBalanceSubscription mockCmd(clientSession);
  MockServerWriter mockSW;

  hub::rpc::BalanceSubscriptionRequest request;

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

  EXPECT_CALL(mockSW, Write(_, _))
      .Times(userAccountEvents.size() + userAddressEvents.size() +
             hubAddressEvents.size())
      .WillRepeatedly(Return(true));
  EXPECT_CALL(mockCmd, getAllUsersAccountBalancesSinceTimePoint(_))
      .Times(1)
      .WillOnce(Return(userAccountEvents));
  EXPECT_CALL(mockCmd, getAllUserAddressesBalancesSinceTimePoint(_))
      .Times(1)
      .WillOnce(Return(userAddressEvents));
  EXPECT_CALL(mockCmd, getAllHubAddressesBalancesSinceTimePoint(_))
      .Times(1)
      .WillOnce(Return(hubAddressEvents));
  ASSERT_TRUE(mockCmd.process(&request, &mockSW).ok());
}

TEST_F(BalanceSubscriptionTest, BalanceSubscriptionExitsOnBrokenStream) {
  auto clientSession = std::make_shared<ClientSession>();
  MockBalanceSubscription mockCmd(clientSession);
  MockServerWriter mockSW;

  hub::rpc::BalanceSubscriptionRequest request;

  std::vector<db::UserAccountBalanceEvent> events;
  events.push_back(db::UserAccountBalanceEvent{});
  events.push_back(db::UserAccountBalanceEvent{});
  events.push_back(db::UserAccountBalanceEvent{});
  events.push_back(db::UserAccountBalanceEvent{});
  EXPECT_CALL(mockSW, Write(_, _)).Times(1).WillOnce(Return(false));
  EXPECT_CALL(mockCmd, getAllUsersAccountBalancesSinceTimePoint(_))
      .Times(1)
      .WillOnce(Return(events));

  ASSERT_FALSE(mockCmd.process(&request, &mockSW).ok());
}
};  // namespace
