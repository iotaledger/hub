// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "common/crypto/types.h"
#include "hub/db/helper.h"
#include "hub/db/types.h"
#include "hub/tests/runner.h"

using namespace hub;
using namespace common::crypto;
using namespace hub::db;

namespace {

class DBTest : public hub::Test {};

TEST_F(DBTest, SweepTriggerWorks) {
  auto& connection = db::DBManager::get().connection();
  auto& cryptoProvider = common::crypto::CryptoManager::get().provider();

  UUID uuid;
  auto hubOutputAddress = cryptoProvider.getAddressForUUID(uuid).value();
  auto hubAddressId = connection.createHubAddress(uuid, hubOutputAddress);
  auto hash = common::crypto::Hash(
      "999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999");

  auto sweepId = connection.createSweep(hash, "", hubAddressId);
  connection.createTail(sweepId, hash.str());

  EXPECT_FALSE(connection.isSweepConfirmed(sweepId));

  connection.markTailAsConfirmed(hash.str());

  EXPECT_TRUE(connection.isSweepConfirmed(sweepId));
}

TEST_F(DBTest, HubAddressTriggerWorks) {
  auto& connection = db::DBManager::get().connection();
  auto& cryptoProvider = common::crypto::CryptoManager::get().provider();
  auto balances = {100, 540, 60, 700};

  uint64_t stepSize = 10;

  UUID uuid;
  auto hubOutputAddress = cryptoProvider.getAddressForUUID(uuid).value();

  std::vector<uint64_t> hubAddressIds;
  for (auto b : balances) {
    auto hubAddressId = connection.createHubAddress(uuid, hubOutputAddress);
    hubAddressIds.push_back(hubAddressId);
  }

  auto sweepId = connection.createSweep(
      common::crypto::Hash(
          "999999999999999999999999999999999999999999999999999999"
          "999999999999999999999999999"),
      "", hubAddressIds[0]);

  uint32_t step = 0;
  for (auto b : balances) {
    for (auto i = 0; i < b / stepSize; ++i) {
      connection.createHubAddressBalanceEntry(hubAddressIds[step], stepSize,
                                              HubAddressBalanceReason::INBOUND,
                                              sweepId);
    }

    auto triggerSummedBalance =
        connection.getHubAddressBalance(hubAddressIds[step]);
    EXPECT_EQ(triggerSummedBalance, b);
    ++step;
  }
}

TEST_F(DBTest, UserAddressTriggerWorks) {
  auto& connection = db::DBManager::get().connection();
  auto& cryptoProvider = common::crypto::CryptoManager::get().provider();

  std::set<std::string> users = {"Ricky", "Stephen", "Karl"};
  std::vector<uint64_t> userBalances = {90000, 10000, 1000};
  std::map<uint64_t, std::string> usersToBalance;
  uint64_t stepSize = 100;

  UUID uuid;
  auto hubOutputAddress = cryptoProvider.getAddressForUUID(uuid).value();
  auto hubAddressId = connection.createHubAddress(uuid, hubOutputAddress);
  auto sweepId = connection.createSweep(
      common::crypto::Hash(
          "999999999999999999999999999999999999999999999999999999"
          "999999999999999999999999999"),
      "", hubAddressId);

  for (auto u : users) {
    connection.createUser(u);
  }

  auto identifiersToIds = connection.userIdsFromIdentifiers(users);
  EXPECT_EQ(identifiersToIds.size(), users.size());

  auto userNum = 0;
  for (const auto& kv : identifiersToIds) {
    UUID uuid;
    auto userAddress = cryptoProvider.getAddressForUUID(uuid).value();
    auto addId = connection.createUserAddress(userAddress, kv.second, uuid);
    usersToBalance[addId] = userBalances[userNum];

    for (auto i = 0; i < userBalances[userNum] / stepSize; ++i) {
      connection.createUserAddressBalanceEntry(
          addId, stepSize, nonstd::nullopt, UserAddressBalanceReason::DEPOSIT,
          {""}, 1);
    }

    // just so all types of REASONs will be tested
    connection.createUserAddressBalanceEntry(addId, -stepSize, nonstd::nullopt,
                                             UserAddressBalanceReason::SWEEP,
                                             {""}, sweepId);

    connection.createUserAddressBalanceEntry(addId, stepSize, nonstd::nullopt,
                                             UserAddressBalanceReason::DEPOSIT,
                                             {""}, 1);

    ++userNum;
  }

  userNum = 0;
  for (const auto& kv : usersToBalance) {
    auto balance = connection.getUserAddressBalance(kv.first);
    EXPECT_EQ(balance, userBalances[userNum++]);
  }
}

};  // namespace
