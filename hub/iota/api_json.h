// Copyright 2018 IOTA Foundation

#ifndef HUB_IOTA_API_JSON_H_
#define HUB_IOTA_API_JSON_H_

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>

#include <nlohmann/json_fwd.hpp>

#include "hub/iota/api.h"

namespace hub {
namespace iota {

class IotaJsonAPI : virtual public IotaAPI {
 public:
  bool isNodeSolid() override;

  std::unordered_map<std::string, uint64_t> getBalances(
      const std::vector<std::string>& addresses) override;

  std::vector<Bundle> getConfirmedBundlesForAddress(
      const std::string& address) override;

  std::unordered_set<std::string> filterConfirmedTails(
      const std::vector<std::string>& tails,
      const std::optional<std::string>& reference) override;

  std::vector<std::string> findTransactions(
      std::optional<std::vector<std::string>> addresses,
      std::optional<std::vector<std::string>> bundles) override;

  std::unordered_set<std::string> filterConsistentTails(
      const std::vector<std::string>& tails) override;

  NodeInfo getNodeInfo() override;
  std::vector<Transaction> getTrytes(
      const std::vector<std::string>& hashes) override;

  std::pair<std::string, std::string> getTransactionsToApprove(
      size_t depth, const std::optional<std::string>& reference = {}) override;

  virtual std::optional<nlohmann::json> post(const nlohmann::json& input) = 0;
};
}  // namespace iota
}  // namespace hub

#endif  // HUB_IOTA_API_JSON_H_
