#ifndef __HUB_IOTA_API_H_
#define __HUB_IOTA_API_H_

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace hub {
namespace iota {

struct Transaction {
  std::string hash;
  std::string address;
  int64_t value;
};

using Bundle = std::vector<Transaction>;

class IotaAPI {
 public:
  virtual std::unordered_map<std::string, uint64_t> getBalances(
      const std::vector<std::string> addresses) = 0;

  virtual std::vector<Bundle> getConfirmedBundlesForAddress(
      const std::string& address) = 0;

  virtual std::unordered_set<std::string> filterConfirmedTails(
      const std::vector<std::string> tails) = 0;
};
}  // namespace iota
}  // namespace hub

#endif /* __HUB_IOTA_API_H_ */
