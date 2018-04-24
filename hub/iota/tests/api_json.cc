#include <glog/logging.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <optional>
#include <thread>

#include "hub/iota/api_json.h"
#include "hub/iota/json.h"

using namespace testing;

using namespace hub;
using namespace hub::iota;
using json = nlohmann::json;

namespace {

class IotaJsonAPITest : public ::testing::Test {};

class MockAPI : public IotaJsonAPI {
 public:
  json req;
  json res;

  std::optional<json> post(const json& in) override {
    LOG(ERROR) << "helllo";

    EXPECT_EQ(req, in);
    req = json{};
    return std::move(res);
  }
};

TEST_F(IotaJsonAPITest, GetBalances) {
  MockAPI api;

  std::string address(
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAA");
  uint64_t balance = 1000;

  std::vector<std::string> addresses = {address};
  std::unordered_map<std::string, uint64_t> expected = {{address, 1000}};

  api.req["command"] = "getBalances";
  api.req["threshold"] = 100;
  api.req["addresses"] = addresses;

  api.res["balances"] = std::vector<std::string>{"1000"};

  auto response = api.getBalances(addresses);

  EXPECT_EQ(response, expected);
}

};  // namespace
