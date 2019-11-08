/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/iota/local_pow.h"

#include "common/helpers/digest.h"
#include "common/helpers/pow.h"
#include "common/trinary/flex_trit.h"
#include "common/trinary/trit_long.h"
#include "common/trinary/tryte_long.h"

#include "common/model/bundle.h"
#include "common/model/transaction.h"

#include <algorithm>
#include <chrono>

#include <glog/logging.h>

namespace hub {
namespace iota {

LocalPOW::LocalPOW(std::shared_ptr<cppclient::IotaAPI> api, size_t depth,
                   size_t mwm)
    : POWProvider(api, depth, mwm) {}

std::vector<std::string> LocalPOW::doPOW(const std::vector<std::string>& trytes,
                                         const std::string& trunk,
                                         const std::string& branch) const {
  bundle_transactions_t* bundle = NULL;
  iota_transaction_t* txIter;
  iota_transaction_t tx;
  std::chrono::system_clock::time_point start =
      std::chrono::system_clock::now();

  bundle_transactions_new(&bundle);

  flex_trit_t txFlexTrits[FLEX_TRIT_SIZE_8019];
  char txTrytesStr[NUM_TRYTES_SERIALIZED_TRANSACTION + 1];
  flex_trit_t trunkTrytes[FLEX_TRIT_SIZE_243];
  flex_trit_t branchTrytes[FLEX_TRIT_SIZE_243];

  std::vector<std::string> powedTxs;
  std::string prevTxHash;

  std::vector<std::string> bundleTxsTrytes;
  flex_trits_from_trytes(trunkTrytes, NUM_TRITS_HASH, (tryte_t*)trunk.c_str(),
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  flex_trits_from_trytes(branchTrytes, NUM_TRITS_HASH, (tryte_t*)branch.c_str(),
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);

  for (auto currTrytes : trytes) {
    for (auto numTx = 0;
         numTx < currTrytes.size() / NUM_TRYTES_SERIALIZED_TRANSACTION;
         numTx++) {
      auto currTxTrytes =
          currTrytes.substr(numTx * NUM_TRYTES_SERIALIZED_TRANSACTION,
                            (numTx + 1) * NUM_TRYTES_SERIALIZED_TRANSACTION);

      flex_trits_from_trytes(txFlexTrits, NUM_TRITS_SERIALIZED_TRANSACTION,
                             (tryte_t*)currTxTrytes.c_str(),
                             NUM_TRYTES_SERIALIZED_TRANSACTION,
                             NUM_TRYTES_SERIALIZED_TRANSACTION);
      transaction_deserialize_from_trits(&tx, txFlexTrits, false);
      bundle_transactions_add(bundle, &tx);
    }
  }

  iota_pow_bundle(bundle, trunkTrytes, branchTrytes, mwm());

  BUNDLE_FOREACH(bundle, txIter) {
    transaction_serialize_on_flex_trits(txIter, txFlexTrits);
    flex_trits_to_trytes(
        (tryte_t*)txTrytesStr, NUM_TRYTES_SERIALIZED_TRANSACTION, txFlexTrits,
        NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRITS_SERIALIZED_TRANSACTION);
    txTrytesStr[NUM_TRYTES_SERIALIZED_TRANSACTION] = 0;
    powedTxs.emplace_back(txTrytesStr);
  }

  std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();

  LOG(INFO) << __FUNCTION__ << " POW took " << duration << " milliseconds";

  bundle_transactions_free(&bundle);

  return powedTxs;
}
}  // namespace iota
}  // namespace hub
