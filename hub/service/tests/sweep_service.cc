// Copyright 2018 IOTA Foundation

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <thread>
#include <vector>
#include <numeric>


#include "hub/service/sweep_service.h"

using namespace testing;

using namespace hub;
using namespace hub::service;

namespace {

class SweepServiceTest : public ::testing::Test {};

class MockService : public SweepService {
 public:
  using SweepService::SweepService;

  const std::string name() const override { return "MockService"; }
  MOCK_METHOD0(doTick, bool());
};

TEST_F(SweepServiceTest, getVecOfMinSizeWithSumNotLessThan) {

    std::vector<db::TransferInput> inputs ;
    std::vector<db::TransferInput> res ;

    common::crypto::UUID uuid;
    common::crypto::Address address(
            "999999999999999999999999999999999999999999999999999999999999999999999999"
            "999999999");

    //{1, 4, 45, 6, 0, 19};
    inputs.emplace_back(hub::db::TransferInput{0,0, address, uuid, 0});
    inputs.emplace_back(hub::db::TransferInput{0,0, address, uuid, 1});
    inputs.emplace_back(hub::db::TransferInput{0,0, address, uuid, 4});
    inputs.emplace_back(hub::db::TransferInput{0,0, address, uuid, 6});
    inputs.emplace_back(hub::db::TransferInput{0,0, address, uuid, 19});
    inputs.emplace_back(hub::db::TransferInput{0,0, address, uuid, 45});

    SweepService::getVecOfMinSizeWithSumNotLessThan(51,inputs,res);

    auto sum = std::accumulate(res.cbegin(), res.cend(), 0uLL,
    [](uint64_t a, const auto& b) { return a + b.amount; });

    EXPECT_EQ(res.size(),2);
    EXPECT_EQ(sum,51);/*45+6*/

    inputs.clear();
    res.clear();
    //{1, 11, 100, 1, 0, 200, 3, 2, 1, 250}
    inputs.emplace_back(hub::db::TransferInput{0,0, address, uuid, 0});
    inputs.emplace_back(hub::db::TransferInput{0,0, address, uuid, 1});
    inputs.emplace_back(hub::db::TransferInput{0,0, address, uuid, 1});
    inputs.emplace_back(hub::db::TransferInput{0,0, address, uuid, 1});
    inputs.emplace_back(hub::db::TransferInput{0,0, address, uuid, 2});
    inputs.emplace_back(hub::db::TransferInput{0,0, address, uuid, 3});
    inputs.emplace_back(hub::db::TransferInput{0,0, address, uuid, 11});
    inputs.emplace_back(hub::db::TransferInput{0,0, address, uuid, 100});
    inputs.emplace_back(hub::db::TransferInput{0,0, address, uuid, 200});
    inputs.emplace_back(hub::db::TransferInput{0,0, address, uuid, 250});

    SweepService::getVecOfMinSizeWithSumNotLessThan(280,inputs,res);

    sum = std::accumulate(res.cbegin(), res.cend(), 0uLL,
                               [](uint64_t a, const auto& b) { return a + b.amount; });

    EXPECT_EQ(res.size(),2);
    EXPECT_EQ(sum,350);

    //check bigger sum than inputs have
    res.clear();
    SweepService::getVecOfMinSizeWithSumNotLessThan(9999,inputs,res);
    EXPECT_EQ(res.size(),inputs.size());

    //check nothing crashes
    inputs.clear();
    res.clear();

    EXPECT_EQ(res.size(),0);



}

};  // namespace