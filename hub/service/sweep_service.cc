// Copyright 2018 IOTA Foundation

#include "hub/service/sweep_service.h"

#include <chrono>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "hub/crypto/manager.h"
#include "hub/crypto/provider.h"
#include "hub/crypto/types.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"

namespace {
DEFINE_uint32(sweep_max_withdraw, 7,
              "Maximum number of withdraw requests to service per sweep.");
DEFINE_uint32(sweep_max_deposit, 5,
              "Maximum number of user deposits to process per sweep.");

}  // namespace

namespace hub {
namespace service {
bool SweepService::doTick() {
  LOG(INFO) << "Starting sweep.";
  auto sweepStart = std::chrono::system_clock::now();

  auto& dbConnection = db::DBManager::get().connection();
  auto& cryptoProvider = crypto::CryptoManager::get().provider();

  auto transaction = dbConnection.transaction();

  try {
    // 1. Get up to X withdrawal requests
    auto outputs = dbConnection.getWithdrawalsForSweep(FLAGS_sweep_max_withdraw,
                                                       sweepStart);

    auto requiredOutput =
        std::accumulate(outputs.cbegin(), outputs.cend(), 0uLL,
                        [](uint64_t a, const auto& b) { return a + b.amount; });

    LOG(INFO) << "Found " << outputs.size()
              << " withdrawals with total amount: " << requiredOutput;

    // 2. Get up to Y sweep inputs (= deposits)
    auto deposits =
        dbConnection.getDepositsForSweep(FLAGS_sweep_max_deposit, sweepStart);
    auto depositInput =
        std::accumulate(deposits.cbegin(), deposits.cend(), 0uLL,
                        [](uint64_t a, const auto& b) { return a + b.amount; });

    LOG(INFO) << "Found " << deposits.size()
              << " deposits with total amount: " << depositInput;

    // 3. Find hub addresses to fill missing input balance.
    std::vector<db::TransferInput> hubInputs;
    uint64_t hubInputTotal = 0;

    if (depositInput < requiredOutput) {
      auto missing = requiredOutput - depositInput;

      LOG(INFO) << "Need to fill " << missing << " via Hub addresses.";

      hubInputs = dbConnection.getHubInputsForSweep(missing, sweepStart);

      hubInputTotal = std::accumulate(
          hubInputs.cbegin(), hubInputs.cend(), 0uLL,
          [](uint64_t a, const auto& b) { return a + b.amount; });

      if (hubInputTotal < missing) {
        LOG(ERROR)
            << "Not enough balance available in hub addresses right now. "
            << "Ignoring withdrawals.";

        requiredOutput = 0;
        outputs.clear();
      }
    }

    LOG(INFO) << "Found " << hubInputs.size()
              << " hub inputs to fill missing funds.";

    // 4. Determine Hub output address
    auto remainder = (hubInputTotal + depositInput) - requiredOutput;
    hub::crypto::UUID hubOutputUUID;
    auto hubOutputAddress = cryptoProvider.getAddressForUUID(hubOutputUUID);

    LOG(INFO) << "Will move " << remainder
              << " into new Hub address: " << hubOutputAddress.str();

    auto hubOutput = db::TransferOutput{
        dbConnection.createHubAddress(hubOutputUUID, hubOutputAddress),
        remainder, std::move(hubOutputAddress)};

    // 5. Generate bundle
    hub::crypto::Hash bundleHash(
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAA");
    std::string bundleTrytes = "9";

    // 6. Commit to DB
    // 6.1. Insert sweep.
    auto sweepId =
        dbConnection.createSweep(bundleHash, bundleTrytes, hubOutput.id);

    // 6.2. Change Hub address balances
    dbConnection.createHubAddressBalanceEntry(
        hubOutput.id, hubOutput.amount, db::HubAddressBalanceReason::INBOUND,
        sweepId);
    for (const auto& input : hubInputs) {
      dbConnection.createHubAddressBalanceEntry(
          input.addressId, -input.amount, db::HubAddressBalanceReason::OUTBOUND,
          sweepId);
    }

    // 6.3. Change User address balances
    for (const auto& input : deposits) {
      dbConnection.createUserAddressBalanceEntry(
          input.addressId, -input.amount, db::UserAddressBalanceReason::SWEEP,
          {}, sweepId);

      dbConnection.createUserAccountBalanceEntry(
          input.userId, input.amount, db::UserAccountBalanceReason::SWEEP,
          sweepId);
    }

    transaction->commit();
  } catch (const std::exception& ex) {
    try {
      transaction->rollback();
    } catch (const std::exception& ex) {
      LOG(ERROR) << "Sweep. Rollback failed: " << ex.what();
    }

    LOG(ERROR) << "Sweep failed: " << ex.what();
  }

  // PoW is handled by attachment service

  return true;
}
}  // namespace service
}  // namespace hub
