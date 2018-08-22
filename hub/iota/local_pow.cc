/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/iota/local_pow.h"

#include "common/helpers/digest.h"
#include "common/helpers/pow.h"
#include "common/trinary/tryte_long.h"

#include <algorithm>
#include <chrono>

#include <glog/logging.h>

namespace hub {
namespace iota {

LocalPOW::LocalPOW(size_t depth, size_t mwm)
    : POWProvider(nullptr, depth, mwm) {}

std::vector<std::string> LocalPOW::doPOW(const std::vector<std::string>& trytes,
                                         const std::string& trunk,
                                         const std::string& branch) const {
  auto timestampSeconds =
      std::chrono::duration_cast<std::chrono::seconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count();

  std::vector<std::string> powedTxs;
  tryte_t trytesTime[9] = {'9', '9', '9', '9', '9', '9', '9', '9', '9'};
  long_to_trytes(timestampSeconds, trytesTime);
  std::string prevTxHash;

  std::chrono::system_clock::time_point start =
      std::chrono::system_clock::now();
  for (auto txTrytes : trytes) {
    txTrytes.replace(TRUNK_OFFSET, 81, prevTxHash.empty() ? trunk : prevTxHash);
    txTrytes.replace(BRANCH_OFFSET, 81, prevTxHash.empty() ? branch : trunk);
    txTrytes.replace(TIMESTAMP_OFFSET, 9, reinterpret_cast<char*>(trytesTime),
                     9);
    auto tag = txTrytes.substr(TAG_OFFSET, 27);
    if (std::all_of(tag.cbegin(), tag.cend(),
                    [&](char c) { return c == '9'; })) {
      txTrytes.replace(TAG_OFFSET, 27,
                       txTrytes.substr(OBSOLETE_TAG_OFFSET, 27));
    }
    char* foundNonce = iota_pow(txTrytes.data(), mwm());
    txTrytes.replace(NONCE_OFFSET, 27, foundNonce);

    char* digest = iota_digest(txTrytes.data());
    prevTxHash = digest;
    free(digest);
    free(foundNonce);
    powedTxs.push_back(std::move(txTrytes));
  }

  std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();

  LOG(INFO) << __FUNCTION__ << " POW took " << duration << " milliseconds";

  return powedTxs;
}
}  // namespace iota
}  // namespace hub
