#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "hub/service/scheduled_service.h"

using namespace testing;

using namespace hub;
using namespace hub::service;

namespace {

class ScheduledServiceTest : public ::testing::Test {};

class MockService : public ScheduledService {
 public:
  using ScheduledService::ScheduledService;

  const std::string name() const override { return "MockService"; }
  MOCK_METHOD0(doTick, bool());
};

TEST_F(ScheduledServiceTest, SchedulingWorks) {
  using namespace std::chrono_literals;
  auto timeout = 100ms;

  MockService service(timeout);

  service.start();
  EXPECT_CALL(service, doTick()).Times(1).WillOnce(Return(false));

  std::this_thread::sleep_for(1.5 * timeout);

  service.stop();
}

};  // namespace
