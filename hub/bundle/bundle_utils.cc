/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/bundle/bundle_utils.h"

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
}  // namespace

namespace hub {
namespace bundle_utils {

std::tuple<common::crypto::Hash, std::string> createBundle(
    const std::vector<db::TransferInput>& deposits,
    const std::vector<db::TransferInput>& hubInputs,
    const std::vector<db::TransferOutput>& withdrawals,
    const db::TransferOutput& hubOutput) {
  auto& cryptoProvider = common::crypto::CryptoManager::get().provider();

  // 5.1. Generate bundle_utils hash & transactions
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

db::TransferOutput getHubOutput(uint64_t remainder) {
  auto& dbConnection = db::DBManager::get().connection();
  auto& cryptoProvider = common::crypto::CryptoManager::get().provider();

  common::crypto::UUID hubOutputUUID;
  auto address = cryptoProvider.getAddressForUUID(hubOutputUUID).value();

  return {dbConnection.createHubAddress(hubOutputUUID, address),
          remainder,
          {},
          std::move(address)};
}

}  // namespace bundle_utils
}  // namespace hub
