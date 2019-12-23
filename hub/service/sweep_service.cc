/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/service/sweep_service.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <glog/logging.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "common/crypto/manager.h"
#include "common/crypto/provider_base.h"
#include "common/crypto/types.h"
#include "hub/bundle/bundle_utils.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"

namespace {
constexpr size_t FRAGMENT_LEN = 2187;
const std::string EMPTY_FRAG(FRAGMENT_LEN, '9');
const std::string EMPTY_NONCE(27, '9');
const std::string EMPTY_HASH(81, '9');

DEFINE_uint32(sweep_max_withdraw, 7,
              "Maximum number of withdraw requests to service per sweep.");
DEFINE_uint32(sweep_max_deposit, 5,
              "Maximum number of user deposits to process per sweep.");

// Hub unswept addresses backup
DEFINE_string(hubSeedsBackupPath, "",
              "Path for file that will be used to backup unswep hub addresses");

}  // namespace

namespace hub {
namespace service {

void SweepService::backupUnsweptHubAddresses() const {
  namespace fs = std::filesystem;
  if (FLAGS_hubSeedsBackupPath.empty()) {
    return;
  }

  std::string tmpFilePath = FLAGS_hubSeedsBackupPath;
  bool replace = false;
  auto& dbConnection = db::DBManager::get().connection();

  if (fs::exists(FLAGS_hubSeedsBackupPath)) {
    tmpFilePath += "_tmp";
    replace = true;
  }

  std::ofstream outputFile;
  // enable exceptions
  outputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    outputFile.open(tmpFilePath);
    auto allHubInputs =
        dbConnection.getAllHubInputs(std::chrono::system_clock::now());

    for (const auto& inp : allHubInputs) {
      outputFile
          << inp.address.str() << ";"
          << common::crypto::CryptoManager::get().provider().getSeedFromUUID(
                 inp.uuid)
          << ";" << inp.amount << "\n";
    }
  } catch (std::ifstream::failure e) {
    outputFile.close();
    if (fs::exists(tmpFilePath)) {
      fs::remove(tmpFilePath);
    }
    LOG(ERROR) << "Failed backing up hub addresses";
    return;
  }

  outputFile.close();

  if (replace) {
    try {
        fs::rename(tmpFilePath, FLAGS_hubSeedsBackupPath);
    } catch (fs::filesystem_error& e) {
      LOG(ERROR) << "Failed renaming old hub addresses backup file";
      return;
    }

  }
}

bool SweepService::doTick() {
  LOG(INFO) << "Starting sweep.";
  auto sweepStart = std::chrono::system_clock::now();

  auto& dbConnection = db::DBManager::get().connection();
  auto transaction = dbConnection.transaction();

  try {
    if (!_api->isNodeSolid()) {
      LOG(INFO) << "Node is not solid. Aborting sweep.";

      transaction->rollback();
      return true;
    }

    // 1. Get up to X withdrawal requests
    auto withdrawals = dbConnection.getWithdrawalsForSweep(
        FLAGS_sweep_max_withdraw, sweepStart);

    auto requiredOutput =
        std::accumulate(withdrawals.cbegin(), withdrawals.cend(), 0uLL,
                        [](uint64_t a, const auto& b) { return a + b.amount; });

    LOG(INFO) << "Found " << withdrawals.size()
              << " withdrawals with total amount: " << requiredOutput;

    // 2. Get up to Y sweep inputs (= deposits)
    auto deposits =
        dbConnection.getDepositsForSweep(FLAGS_sweep_max_deposit, sweepStart);
    auto depositsTotal =
        std::accumulate(deposits.cbegin(), deposits.cend(), 0uLL,
                        [](uint64_t a, const auto& b) { return a + b.amount; });

    LOG(INFO) << "Found " << deposits.size()
              << " deposits with total amount: " << depositsTotal;

    // 2.1. Abort if no deposits and no withdrawals were found.
    if (deposits.size() == 0 && withdrawals.size() == 0) {
      LOG(INFO) << "No deposits or withdrawal requests found. Aborting sweep.";

      transaction->rollback();
      return true;
    }

    // 3. Find hub addresses to fill missing input balance.
    std::vector<db::TransferInput> hubInputs;
    uint64_t hubInputTotal = 0;

    if (depositsTotal < requiredOutput) {
      auto missing = requiredOutput - depositsTotal;

      LOG(INFO) << "Need to fill " << missing << " via Hub addresses.";

      hubInputs = dbConnection.getHubInputsForSweep(missing, sweepStart);

      hubInputTotal = std::accumulate(
          hubInputs.cbegin(), hubInputs.cend(), 0uLL,
          [](uint64_t a, const auto& b) { return a + b.amount; });

      if (hubInputTotal < missing) {
        LOG(ERROR)
            << "Not enough balance available in hub addresses right now. "
            << "Needed: " << missing << " but only found: " << hubInputTotal
            << ". Ignoring withdrawals.";

        hubInputTotal = 0;
        hubInputs.clear();
        requiredOutput = 0;
        withdrawals.clear();
      }
    }

    // 3.1. Abort if no deposits or withdrawals found.
    if (deposits.size() == 0 && withdrawals.size() == 0) {
      LOG(INFO) << "No deposits or withdrawal requests found. Aborting sweep.";

      transaction->rollback();
      return true;
    }

    LOG(INFO) << "Found " << hubInputs.size()
              << " hub inputs to fill missing funds.";

    // 4. Determine Hub output address
    auto remainder = (hubInputTotal + depositsTotal) - requiredOutput;
    auto hubOutput = hub::bundle_utils::getHubOutput(remainder);

    LOG(INFO) << "Will move " << remainder
              << " into new Hub address: " << hubOutput.payoutAddress.str();
    // 5. Generate bundle_utils
    auto bundle = bundle_utils::createBundle(deposits, hubInputs, withdrawals,
                                             {hubOutput});

    // 6. Commit to DB
    hub::bundle_utils::persistToDatabase(bundle, deposits, hubInputs,
                                         withdrawals, {hubOutput});

    if (remainder > 0) {
      backupUnsweptHubAddresses();
    }

    transaction->commit();
    LOG(INFO) << "Sweep complete.";
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
