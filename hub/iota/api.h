// Copyright 2018 IOTA Foundation

#ifndef HUB_IOTA_API_H_
#define HUB_IOTA_API_H_

#include <chrono>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace hub {
namespace iota {

struct Transaction {
 public:
  std::string hash;
  std::string address;
  int64_t value;
  std::chrono::system_clock::time_point timestamp;
  int64_t currentIndex;
  int64_t lastIndex;
  std::string bundleHash;
  std::string trunk;
};

struct NodeInfo {
 public:
  std::string latestMilestone;
  uint64_t latestMilestoneIndex;
  uint64_t latestSolidMilestoneIndex;
};

using Bundle = std::vector<Transaction>;

class IotaAPI {
 public:
  virtual bool isNodeSolid() = 0;

  virtual std::unordered_map<std::string, uint64_t> getBalances(
      const std::vector<std::string>& addresses) = 0;

  virtual std::vector<Bundle> getConfirmedBundlesForAddress(
      const std::string& address) = 0;

  virtual std::unordered_set<std::string> filterConfirmedTails(
      const std::vector<std::string>& tails,
      const std::optional<std::string>& reference = {}) = 0;

  virtual std::unordered_set<std::string> filterConsistentTails(
      const std::vector<std::string>& tails) = 0;

  virtual std::vector<std::string> findTransactions(
      std::optional<std::vector<std::string>> addresses,
      std::optional<std::vector<std::string>> bundles) = 0;

  virtual NodeInfo getNodeInfo() = 0;
  virtual std::vector<Transaction> getTrytes(
      const std::vector<std::string>& hashes) = 0;

  virtual std::pair<std::string, std::string> getTransactionsToApprove(
      size_t depth, const std::optional<std::string>& reference = {}) = 0;
};
}  // namespace iota
}  // namespace hub

#endif  // HUB_IOTA_API_H_
