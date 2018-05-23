/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */


#ifndef HUB_IOTA_API_JSON_H_
#define HUB_IOTA_API_JSON_H_

#include <cstdint>
#include <nonstd/optional.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <nlohmann/json_fwd.hpp>

#include "hub/iota/api.h"

namespace hub {
namespace iota {

/// Implementation of IotaAPI class with JSON responses
class IotaJsonAPI : virtual public IotaAPI {
 public:
  bool isNodeSolid() override;

  std::unordered_map<std::string, uint64_t> getBalances(
      const std::vector<std::string>& addresses) override;

  std::unordered_multimap<std::string, Bundle> getConfirmedBundlesForAddresses(
      const std::vector<std::string>& addresses) override;

  std::unordered_set<std::string> filterConfirmedTails(
      const std::vector<std::string>& tails,
      const nonstd::optional<std::string>& reference) override;

  std::vector<std::string> findTransactions(
      nonstd::optional<std::vector<std::string>> addresses,
      nonstd::optional<std::vector<std::string>> bundles) override;

  std::unordered_set<std::string> filterConsistentTails(
      const std::vector<std::string>& tails) override;

  NodeInfo getNodeInfo() override;
  std::vector<Transaction> getTrytes(
      const std::vector<std::string>& hashes) override;

  std::pair<std::string, std::string> getTransactionsToApprove(
      size_t depth,
      const nonstd::optional<std::string>& reference = {}) override;

  std::vector<std::string> attachToTangle(
      const std::string& trunkTransaction, const std::string& branchTransaction,
      size_t minWeightMagnitude,
      const std::vector<std::string>& trytes) override;

  bool storeTransactions(const std::vector<std::string>& trytes) override;
  bool broadcastTransactions(const std::vector<std::string>& trytes) override;

  virtual nonstd::optional<nlohmann::json> post(
      const nlohmann::json& input) = 0;
};
}  // namespace iota
}  // namespace hub

#endif  // HUB_IOTA_API_JSON_H_
