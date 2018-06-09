// Copyright 2018 IOTA Foundation

#include <chrono>
#include <iostream>
#include <nonstd/optional.hpp>
#include <thread>

#include <glog/logging.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "hub/iota/api_json.h"

using namespace testing;

using namespace hub;
using namespace hub::iota;
using json = nlohmann::json;

namespace {

class IotaJsonAPITest : public ::testing::Test {};

class MockAPI : public IotaJsonAPI {
 public:
  nonstd::optional<json> post(const json& in) override {
    auto res = post_(in.dump());

    if (res) {
      return json::parse(*res);
    }

    return {};
  }

  MOCK_METHOD1(post_, nonstd::optional<std::string>(const std::string&));
};

TEST_F(IotaJsonAPITest, GetBalances) {
  MockAPI api;

  std::string address(
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAA");
  uint64_t balance = 1000;

  std::vector<std::string> addresses = {address};
  std::unordered_map<std::string, uint64_t> expected = {{address, 1000}};

  json req;
  req["command"] = "getBalances";
  req["threshold"] = 100;
  req["addresses"] = addresses;

  json res;
  res["balances"] = std::vector<std::string>{"1000"};

  EXPECT_CALL(api, post_(req.dump()))
      .Times(1)
      .WillOnce(Return(nonstd::optional<std::string>{res.dump()}));

  auto response = api.getBalances(addresses);

  EXPECT_EQ(response, expected);
}

};  // namespace
