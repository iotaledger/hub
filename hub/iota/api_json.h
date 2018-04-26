// Copyright 2018 IOTA Foundation

#ifndef HUB_IOTA_API_JSON_H_
#define HUB_IOTA_API_JSON_H_

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <nlohmann/json_fwd.hpp>

#include "hub/iota/api.h"

namespace hub {
namespace iota {

struct NodeInfo {
 public:
  std::string latestMilestone;
  uint64_t latestMilestoneIndex;
  uint64_t latestSolidMilestoneIndex;
};

class IotaJsonAPI : virtual public IotaAPI {
 public:
  bool isNodeSolid() override;

  std::unordered_map<std::string, uint64_t> getBalances(
      const std::vector<std::string>& addresses) override;

  std::vector<Bundle> getConfirmedBundlesForAddress(
      const std::string& address) override;

  std::unordered_set<std::string> filterConfirmedTails(
      const std::vector<std::string>& tails) override;

  std::vector<std::string> findTransactions(
      std::optional<std::vector<std::string>> addresses,
      std::optional<std::vector<std::string>> bundles);

  NodeInfo getNodeInfo();
  std::vector<Transaction> getTrytes(const std::vector<std::string>& hashes);

  virtual std::optional<nlohmann::json> post(const nlohmann::json& input) = 0;
};
}  // namespace iota
}  // namespace hub

#endif  // HUB_IOTA_API_JSON_H_
