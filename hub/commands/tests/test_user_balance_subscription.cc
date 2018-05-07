// Copyright 2018 IOTA Foundation

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <chrono>
#include <thread>

#include "hub/commands/user_balance_subscription.h"
#include "hub/stats/session.h"
#include "proto/hub.grpc.pb.h"
#include "runner.h"

using namespace testing;
using namespace hub;
using namespace hub::cmd;

namespace {

class UserBalanceSubscriptionTest : public CommandTest {};

class MockUserBalanceSubscription : public UserBalanceSubscription {
 public:
  using UserBalanceSubscription::doProcess;
  using UserBalanceSubscription::UserBalanceSubscription;
  using IWriter = grpc::ServerWriterInterface<rpc::UserBalanceEvent>;
  MOCK_METHOD1(getAccountBalances,
               std::vector<db::UserBalanceEvent>(
                   std::chrono::system_clock::time_point lastCheck));
};

class MockServerWriter
    : public grpc::ServerWriterInterface<rpc::UserBalanceEvent> {
 public:
  MOCK_METHOD0(SendInitialMetadata, void(void));
  MOCK_METHOD2(Write,
               bool(const hub::rpc::UserBalanceEvent& msg, grpc::WriteOptions));
  // bool 	Write (const W &msg) - Is not virtual, but the method above is,
  // and will be called internally when the non virtual version is called
};

TEST_F(UserBalanceSubscriptionTest, UserBalanceSubscriptionWriteAllEvents) {
  auto clientSession = std::make_shared<ClientSession>();
  MockUserBalanceSubscription mockCmd(clientSession);
  MockServerWriter mockSW;

  hub::rpc::UserBalanceSubscriptionRequest request;

  std::vector<db::UserBalanceEvent> events;
  events.push_back(db::UserBalanceEvent{});
  events.push_back(db::UserBalanceEvent{});
  events.push_back(db::UserBalanceEvent{});
  events.push_back(db::UserBalanceEvent{});
  EXPECT_CALL(mockSW, Write(_, _))
      .Times(events.size())
      .WillRepeatedly(Return(true));
  EXPECT_CALL(mockCmd, getAccountBalances(_)).Times(1).WillOnce(Return(events));
  mockCmd.doProcess(&request, &mockSW);
}

TEST_F(UserBalanceSubscriptionTest,
       UserBalanceSubscriptionExitsOnBrokenStream) {
  auto clientSession = std::make_shared<ClientSession>();
  MockUserBalanceSubscription mockCmd(clientSession);
  MockServerWriter mockSW;

  hub::rpc::UserBalanceSubscriptionRequest request;

  std::vector<db::UserBalanceEvent> events;
  events.push_back(db::UserBalanceEvent{});
  events.push_back(db::UserBalanceEvent{});
  events.push_back(db::UserBalanceEvent{});
  events.push_back(db::UserBalanceEvent{});
  EXPECT_CALL(mockSW, Write(_, _)).Times(1).WillOnce(Return(false));
  EXPECT_CALL(mockCmd, getAccountBalances(_)).Times(1).WillOnce(Return(events));
  mockCmd.doProcess(&request, &mockSW);
}
};  // namespace
