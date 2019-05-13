/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/service/sweep_service.h"

#include <chrono>
#include <numeric>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <iota/models/bundle.hpp>
#include <iota/models/transaction.hpp>

#include "common/crypto/manager.h"
#include "common/crypto/provider_base.h"
#include "common/crypto/types.h"
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

}  // namespace

namespace hub {
namespace service {

db::TransferOutput SweepService::getHubOutput(uint64_t remainder) {
  auto& dbConnection = db::DBManager::get().connection();
  auto& cryptoProvider = common::crypto::CryptoManager::get().provider();

  common::crypto::UUID hubOutputUUID;
  auto address = cryptoProvider.getAddressForUUID(hubOutputUUID).value();

  return {dbConnection.createHubAddress(hubOutputUUID, address),
          remainder,
          {},
          std::move(address)};
}

std::tuple<common::crypto::Hash, std::string> SweepService::createBundle(
    const std::vector<db::TransferInput>& deposits,
    const std::vector<db::TransferInput>& hubInputs,
    const std::vector<db::TransferOutput>& withdrawals,
    const db::TransferOutput& hubOutput) {
  auto& dbConnection = db::DBManager::get().connection();
  auto& cryptoProvider = common::crypto::CryptoManager::get().provider();

  // 5.1. Generate bundle hash & transactions
  IOTA::Models::Bundle bundle;
  {
    // timestamp.
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                         std::chrono::system_clock::now().time_since_epoch())
                         .count();

    // inputs: deposits
    for (const auto& deposit : deposits) {
      IOTA::Models::Transaction tx;
      tx.setAddress(deposit.address.str());
      tx.setTimestamp(timestamp);
      tx.setValue((-1uLL) * deposit.amount);

      bundle.addTransaction(tx,
                            cryptoProvider.securityLevel(deposit.uuid).value());
    }
    // inputs: hubInputs
    for (const auto& input : hubInputs) {
      IOTA::Models::Transaction tx;
      tx.setAddress(input.address.str());
      tx.setTimestamp(timestamp);
      tx.setValue((-1uLL) * input.amount);

      bundle.addTransaction(tx,
                            cryptoProvider.securityLevel(input.uuid).value());
    }
    // outputs: withdrawals
    for (const auto& wd : withdrawals) {
      IOTA::Models::Transaction tx;
      tx.setAddress(wd.payoutAddress.str());
      tx.setTimestamp(timestamp);
      tx.setValue(wd.amount);

      if (wd.tag) {
        tx.setObsoleteTag(wd.tag->str());
      }

      bundle.addTransaction(tx, 1);
    }

    // output: hubOutput
    IOTA::Models::Transaction tx;
    tx.setAddress(hubOutput.payoutAddress.str());
    tx.setTimestamp(timestamp);
    tx.setValue(hubOutput.amount);

    bundle.addTransaction(tx, 1);
  }

  bundle.finalize();

  common::crypto::Hash bundleHash(bundle.getHash());

  // 5.2 Generate signatures
  std::unordered_map<common::crypto::Address, std::string> signaturesForAddress;

  for (const auto& in : deposits) {
    signaturesForAddress[in.address] =
        cryptoProvider.getSignatureForUUID(in.uuid, bundleHash).value();
  }
  for (const auto& in : hubInputs) {
    signaturesForAddress[in.address] =
        cryptoProvider.getSignatureForUUID(in.uuid, bundleHash).value();
  }

  auto it = bundle.getTransactions().begin();

  while (it != bundle.getTransactions().end()) {
    auto& tx = *it;

    if (tx.getValue() >= 0) {
      tx.setSignatureFragments(EMPTY_FRAG);
      tx.setNonce(EMPTY_NONCE);
      tx.setTrunkTransaction(EMPTY_HASH);
      tx.setBranchTransaction(EMPTY_HASH);

      if (tx.getObsoleteTag().empty()) {
        tx.setObsoleteTag(EMPTY_NONCE);
      }

      ++it;
      continue;
    }

    std::string_view signature = signaturesForAddress.at(
        common::crypto::Address(tx.getAddress().toTrytes()));

    while (!signature.empty()) {
      (*it).setSignatureFragments(
          std::string(signature.substr(0, FRAGMENT_LEN)));
      (*it).setNonce(EMPTY_NONCE);
      (*it).setTrunkTransaction(EMPTY_HASH);
      (*it).setBranchTransaction(EMPTY_HASH);

      signature.remove_prefix(FRAGMENT_LEN);

      ++it;
    }
  }

  std::ostringstream bundleTrytesOS;
  for (const auto& tx : bundle.getTransactions()) {
    bundleTrytesOS << tx.toTrytes();
  }

  return {std::move(bundleHash), bundleTrytesOS.str()};
}

void SweepService::persistToDatabase(
    std::tuple<common::crypto::Hash, std::string> bundle,
    const std::vector<db::TransferInput>& deposits,
    const std::vector<db::TransferInput>& hubInputs,
    const std::vector<db::TransferOutput>& withdrawals,
    const db::TransferOutput& hubOutput) {
  auto& dbConnection = db::DBManager::get().connection();

  // 6.1. Insert sweep.
  auto sweepId = dbConnection.createSweep(std::get<0>(bundle),
                                          std::get<1>(bundle), hubOutput.id);

  // 6.2. Change Hub address balances

  dbConnection.createHubAddressBalanceEntry(
      hubOutput.id, hubOutput.amount, db::HubAddressBalanceReason::INBOUND,
      sweepId);
  for (const auto& input : hubInputs) {
    dbConnection.createHubAddressBalanceEntry(
        input.addressId, -input.amount, db::HubAddressBalanceReason::OUTBOUND,
        sweepId);
  }

  // 6.3. Update withdrawal sweep id
  for (const auto& withdrawal : withdrawals) {
    dbConnection.setWithdrawalSweep(withdrawal.id, sweepId);
  }

  // 6.4. Change User address balances
  for (const auto& input : deposits) {
    dbConnection.createUserAddressBalanceEntry(
        input.addressId, -input.amount, nonstd::nullopt,
        db::UserAddressBalanceReason::SWEEP, {}, sweepId);

    dbConnection.createUserAccountBalanceEntry(
        input.userId, input.amount, db::UserAccountBalanceReason::SWEEP,
        sweepId);
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
    auto hubOutput = getHubOutput(remainder);

    LOG(INFO) << "Will move " << remainder
              << " into new Hub address: " << hubOutput.payoutAddress.str();
    // 5. Generate bundle
    auto bundle = createBundle(deposits, hubInputs, withdrawals, hubOutput);

    // 6. Commit to DB
    persistToDatabase(bundle, deposits, hubInputs, withdrawals, hubOutput);

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
