/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
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

#include "common/model/bundle.h"
#include "common/model/transaction.h"
#include "common/trinary/trit_long.h"
#include "common/trinary/tryte_long.h"
#include "utils/bundle_miner.h"

#include "common/crypto/manager.h"
#include "common/crypto/provider_base.h"
#include "common/crypto/types.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"

DEFINE_uint32(numBundlesToMine, 5000000,
              "number of different bundles to mine for generating a signature "
              "for already spent addresses");

namespace {

constexpr size_t FRAGMENT_LEN = 2187;
const std::string EMPTY_FRAG(FRAGMENT_LEN, '9');
const std::string EMPTY_NONCE(27, '9');
const std::string EMPTY_HASH(81, '9');
}  // namespace

namespace hub {
namespace bundle_utils {

static void mineBundle(
    bundle_transactions_t* const bundle,
    const std::vector<std::string>& alreadySignedBundleHashes,
    size_t security) {
  byte_t current_bundle_bytes[NORMALIZED_BUNDLE_LENGTH];
  byte_t current_already_signed_bytes[NORMALIZED_BUNDLE_LENGTH];
  byte_t min[NORMALIZED_BUNDLE_LENGTH];
  size_t const essence_length = 486 * bundle_transactions_size(bundle);
  trit_t essence[essence_length];
  flex_trit_t tagFlexTrits[FLEX_TRIT_SIZE_81];
  trit_t tagTrits[NUM_TRITS_TAG];
  uint64_t index = 0;

  trit_t current_bundle_trits[HASH_LENGTH_TRIT];
  trit_t current_already_signed_bundle_trits[HASH_LENGTH_TRIT];

  flex_trits_to_trits(current_bundle_trits, HASH_LENGTH_TRIT,
                      bundle_transactions_bundle_hash(bundle), HASH_LENGTH_TRIT,
                      HASH_LENGTH_TRIT);

  normalize_hash(current_bundle_trits, current_bundle_bytes);

  for (auto signedHash : alreadySignedBundleHashes) {
    memset(current_already_signed_bytes, 0, NORMALIZED_BUNDLE_LENGTH);
    trytes_to_trits((tryte_t*)signedHash.c_str(),
                    current_already_signed_bundle_trits, HASH_LENGTH_TRYTE);
    memset(min, 0, NORMALIZED_BUNDLE_LENGTH);
    normalize_hash(current_already_signed_bundle_trits,
                   current_already_signed_bytes);
    bundle_miner_normalized_bundle_max(current_bundle_bytes,
                                       current_already_signed_bytes, min,
                                       NORMALIZED_BUNDLE_LENGTH);
    memcpy(current_bundle_bytes, min, NORMALIZED_BUNDLE_LENGTH);
  }

  iota_transaction_t* txIter;

  memset(essence, 0, essence_length);

  BUNDLE_FOREACH(bundle, txIter) {
    trit_t* curr_pos = essence;

    flex_trits_to_trits(curr_pos, NUM_TRITS_ADDRESS,
                        transaction_address(txIter), NUM_TRITS_ADDRESS,
                        NUM_TRITS_ADDRESS);
    curr_pos = &curr_pos[NUM_TRITS_ADDRESS];
    long_to_trits(transaction_value(txIter), curr_pos);
    curr_pos = &curr_pos[NUM_TRITS_VALUE];
    flex_trits_to_trits(curr_pos, NUM_TRITS_OBSOLETE_TAG,
                        transaction_obsolete_tag(txIter),
                        NUM_TRITS_OBSOLETE_TAG, NUM_TRITS_OBSOLETE_TAG);
    curr_pos = &curr_pos[NUM_TRITS_OBSOLETE_TAG];
    long_to_trits(transaction_timestamp(txIter), curr_pos);
    curr_pos = &curr_pos[NUM_TRITS_TIMESTAMP];
    long_to_trits(transaction_current_index(txIter), curr_pos);
    curr_pos = &curr_pos[NUM_TRITS_CURRENT_INDEX];
    long_to_trits(transaction_last_index(txIter), curr_pos);
  }

  LOG(INFO) << "Mining for secure bundle began";
  bundle_miner_mine(min, security, essence, essence_length,
                    FLAGS_numBundlesToMine, 0, &index);
  LOG(INFO) << "Mining for secure bundle finished";

  txIter = bundle_at(bundle, 0);

  memset(tagTrits, 0, NUM_TRITS_TAG);
  long_to_trits(index, tagTrits);
  flex_trits_from_trits(tagFlexTrits, NUM_TRITS_TAG, tagTrits, NUM_TRITS_TAG,
                        NUM_TRITS_TAG);
  transaction_set_obsolete_tag(txIter, tagFlexTrits);
}

std::tuple<common::crypto::Hash, std::string> createBundle(
    const std::vector<db::TransferInput>& deposits,
    const std::vector<db::TransferInput>& hubInputs,
    const std::vector<db::TransferOutput>& withdrawals,
    const nonstd::optional<db::TransferOutput> hubOutputOptional,
    const std::vector<std::string>& alreadySignedBundleHashes) {
  auto& cryptoProvider = common::crypto::CryptoManager::get().provider();

  // 5.1. Generate bundle_utils hash & transactions
  bundle_transactions_t* bundle = NULL;
  iota_transaction_t* txIter;
  bundle_transactions_new(&bundle);
  flex_trit_t addressFlexTrits[FLEX_TRIT_SIZE_243];
  flex_trit_t tagFlexTrits[FLEX_TRIT_SIZE_81];
  flex_trit_t sigFlexTrits[FLEX_TRIT_SIZE_6561];

  char bundleHashStr[NUM_TRYTES_HASH + 1];
  char addressStr[NUM_TRYTES_ADDRESS + 1];

  Kerl kerl;
  kerl_init(&kerl);
  {
    // timestamp.
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                         std::chrono::system_clock::now().time_since_epoch())
                         .count();

    // inputs: deposits
    for (const auto& deposit : deposits) {
      iota_transaction_t txE;
      transaction_reset(&txE);
      transaction_set_value(&txE, (-1uLL) * deposit.amount);
      flex_trits_from_trytes(addressFlexTrits, NUM_TRITS_ADDRESS,
                             (tryte_t*)deposit.address.str().c_str(),
                             NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);
      transaction_set_address(&txE, addressFlexTrits);
      transaction_set_timestamp(&txE, timestamp);
      txE.loaded_columns_mask.attachment |= MASK_ATTACHMENT_TAG;
      txE.loaded_columns_mask.essence |= MASK_ESSENCE_OBSOLETE_TAG;

      bundle_transactions_add(bundle, &txE);
      transaction_set_value(&txE, 0);
      for (auto txCount = 1;
           txCount < cryptoProvider.securityLevel(deposit.uuid).value();
           ++txCount) {
        bundle_transactions_add(bundle, &txE);
      }
    }
    // inputs: hubInputs
    for (const auto& input : hubInputs) {
      iota_transaction_t txE;
      transaction_reset(&txE);
      transaction_set_value(&txE, (-1uLL) * input.amount);
      flex_trits_from_trytes(addressFlexTrits, NUM_TRITS_ADDRESS,
                             (tryte_t*)input.address.str().c_str(),
                             NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);
      transaction_set_address(&txE, addressFlexTrits);
      transaction_set_timestamp(&txE, timestamp);
      txE.loaded_columns_mask.attachment |= MASK_ATTACHMENT_TAG;
      txE.loaded_columns_mask.essence |= MASK_ESSENCE_OBSOLETE_TAG;

      bundle_transactions_add(bundle, &txE);
      transaction_set_value(&txE, 0);
      for (auto txCount = 1;
           txCount < cryptoProvider.securityLevel(input.uuid).value();
           ++txCount) {
        bundle_transactions_add(bundle, &txE);
      }
    }
    // outputs: withdrawals
    for (const auto& wd : withdrawals) {
      iota_transaction_t txE;
      transaction_reset(&txE);
      transaction_set_value(&txE, wd.amount);
      flex_trits_from_trytes(addressFlexTrits, NUM_TRITS_ADDRESS,
                             (tryte_t*)wd.payoutAddress.str().c_str(),
                             NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);
      transaction_set_address(&txE, addressFlexTrits);

      if (wd.tag) {
        flex_trits_from_trytes(tagFlexTrits, NUM_TRITS_TAG,
                               (tryte_t*)wd.tag->str().c_str(), NUM_TRYTES_TAG,
                               NUM_TRYTES_TAG);
        transaction_set_tag(&txE, tagFlexTrits);
      }

      transaction_set_timestamp(&txE, timestamp);
      txE.loaded_columns_mask.attachment |= MASK_ATTACHMENT_TAG;
      txE.loaded_columns_mask.essence |= MASK_ESSENCE_OBSOLETE_TAG;

      bundle_transactions_add(bundle, &txE);
    }

    // output: hubOutput
    if (hubOutputOptional) {
      iota_transaction_t txE;
      transaction_reset(&txE);
      transaction_set_value(&txE, hubOutputOptional->amount);
      flex_trits_from_trytes(
          addressFlexTrits, NUM_TRITS_ADDRESS,
          (tryte_t*)hubOutputOptional->payoutAddress.str().c_str(),
          NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);
      transaction_set_address(&txE, addressFlexTrits);
      transaction_set_timestamp(&txE, timestamp);
      txE.loaded_columns_mask.attachment |= MASK_ATTACHMENT_TAG;
      txE.loaded_columns_mask.essence |= MASK_ESSENCE_OBSOLETE_TAG;

      bundle_transactions_add(bundle, &txE);
    }
  }

  bundle_reset_indexes(bundle);
  bundle_finalize(bundle, &kerl);

  if (!alreadySignedBundleHashes.empty()) {
    // Since mining will only be required from "RecoverFunds", there's only one
    // deposit that reuses the address
    auto security = cryptoProvider.securityLevel(deposits.front().uuid).value();
    mineBundle(bundle, alreadySignedBundleHashes, security);
    kerl_init(&kerl);
    bundle_finalize(bundle, &kerl);
  }

  flex_trits_to_trytes((tryte_t*)bundleHashStr, NUM_TRYTES_HASH,
                       bundle_transactions_bundle_hash(bundle), NUM_TRITS_HASH,
                       NUM_TRITS_HASH);
  bundleHashStr[NUM_TRYTES_HASH] = 0;

  common::crypto::Hash bundleHash(bundleHashStr);

  // 5.2 Generate signatures
  std::unordered_map<common::crypto::Address, std::string> signaturesForAddress;

  for (const auto& in : deposits) {
    if (!alreadySignedBundleHashes.empty()) {
      signaturesForAddress[in.address] =
          cryptoProvider.forceGetSignatureForUUID(in.uuid, bundleHash).value();
    } else {
      signaturesForAddress[in.address] =
          cryptoProvider.getSignatureForUUID(in.uuid, bundleHash).value();
    }
  }
  for (const auto& in : hubInputs) {
    signaturesForAddress[in.address] =
        cryptoProvider.getSignatureForUUID(in.uuid, bundleHash).value();
  }

  BUNDLE_FOREACH(bundle, txIter) {
    if (transaction_value(txIter) >= 0) {
      continue;
    }

    flex_trits_to_trytes((tryte_t*)addressStr, NUM_TRYTES_ADDRESS,
                         transaction_address(txIter), NUM_TRITS_ADDRESS,
                         NUM_TRITS_ADDRESS);
    addressStr[NUM_TRYTES_ADDRESS] = 0;
    std::string_view signature =
        signaturesForAddress.at(common::crypto::Address(addressStr));

    while (!signature.empty()) {
      flex_trits_from_trytes(
          sigFlexTrits, NUM_TRITS_SIGNATURE,
          (tryte_t*)std::string(signature.substr(0, FRAGMENT_LEN)).c_str(),
          NUM_TRYTES_SIGNATURE, NUM_TRYTES_SIGNATURE);
      transaction_set_signature(txIter, sigFlexTrits);
      signature.remove_prefix(FRAGMENT_LEN);

      txIter = (iota_transaction_t*)utarray_next(bundle, txIter);
    }
  }

  std::ostringstream bundleTrytesOS;

  flex_trit_t txFlexTrits[FLEX_TRIT_SIZE_8019];
  char txTrytesStr[NUM_TRYTES_SERIALIZED_TRANSACTION + 1];
  BUNDLE_FOREACH(bundle, txIter) {
    transaction_serialize_on_flex_trits(txIter, txFlexTrits);
    flex_trits_to_trytes(
        (tryte_t*)txTrytesStr, NUM_TRYTES_SERIALIZED_TRANSACTION, txFlexTrits,
        NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRITS_SERIALIZED_TRANSACTION);
    txTrytesStr[NUM_TRYTES_SERIALIZED_TRANSACTION] = 0;
    bundleTrytesOS << txTrytesStr;
  }

  bundle_transactions_free(&bundle);

  return {std::move(bundleHash), bundleTrytesOS.str()};
}

}  // namespace bundle_utils
}  // namespace hub
