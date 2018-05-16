// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "hub/crypto/types.h"
#include "hub/db/helper.h"
#include "hub/db/types.h"
#include "hub/tests/runner.h"

using namespace hub;
using namespace hub::crypto;
using namespace hub::db;

namespace {

class DBTest : public hub::Test {};

TEST_F(DBTest, HubAddressTriggerWorks) {
  auto& connection = db::DBManager::get().connection();
  auto& cryptoProvider = crypto::CryptoManager::get().provider();
  auto balances = {100, 540, 60, 700};

  uint64_t stepSize = 10;

  UUID uuid;
  auto hubOutputAddress = cryptoProvider.getAddressForUUID(uuid);

  std::vector<uint64_t> hubAddressIds;
  for (auto b : balances) {
    auto hubAddressId =
        connection.createHubAddress(uuid, hubOutputAddress.str());
    hubAddressIds.push_back(hubAddressId);
  }

  uint32_t step = 0;
  for (auto b : balances) {
    for (auto i = 0; i < b / stepSize; ++i) {
      connection.createHubAddressBalanceEntry(
          hubAddressIds[step], stepSize, HubAddressBalanceReason::INBOUND, 1);
    }

    auto triggerSummedBalance =
        connection.getHubAddressBalance(hubAddressIds[step]);
    EXPECT_EQ(triggerSummedBalance, b);
    ++step;
  }
}

TEST_F(DBTest, UserAddressTriggerWorks) {
  auto& connection = db::DBManager::get().connection();
  auto& cryptoProvider = crypto::CryptoManager::get().provider();

  std::set<std::string> users = {"Ricky", "Stephen", "Karl"};
  std::vector<uint64_t> userBalances = {90000, 10000, 1000};
  std::map<uint64_t, std::string> usersToBalance;
  uint64_t stepSize = 100;

  for (auto u : users) {
    connection.createUser(u);
  }

  auto identifiersToIds = connection.userIdsFromIdentifiers(users);
  EXPECT_EQ(identifiersToIds.size(), users.size());

  auto userNum = 0;
  for (const auto& kv : identifiersToIds) {
    UUID uuid;
    auto userAddress = cryptoProvider.getAddressForUUID(uuid);
    auto addId = connection.createUserAddress(userAddress, kv.second, uuid);
    usersToBalance[addId] = userBalances[userNum];

    for (auto i = 0; i < userBalances[userNum] / stepSize; ++i) {
      connection.createUserAddressBalanceEntry(
          addId, stepSize, UserAddressBalanceReason::DEPOSIT, "", 1);
    }

    // just so all types of REASONs will be tested
    connection.createUserAddressBalanceEntry(
        addId, -stepSize, UserAddressBalanceReason::SWEEP, "", 1);

    connection.createUserAddressBalanceEntry(
        addId, stepSize, UserAddressBalanceReason::DEPOSIT, "", 1);

    ++userNum;
  }

  userNum = 0;
  for (const auto& kv : usersToBalance) {
    auto balance = connection.getUserAddressBalance(kv.first);
    EXPECT_EQ(balance, userBalances[userNum++]);
  }
}


};  // namespace
