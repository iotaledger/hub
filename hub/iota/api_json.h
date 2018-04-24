#ifndef __HUB_IOTA_API_JSON_H_
#define __HUB_IOTA_API_JSON_H_

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "api.h"
#include "json.h"

namespace hub {
namespace iota {

class IotaJsonAPI : virtual public IotaAPI {
 public:
  std::unordered_map<std::string, uint64_t> getBalances(
      const std::vector<std::string>& addresses) override;

  std::vector<Bundle> getConfirmedBundlesForAddress(
      const std::string& address) override;

  std::unordered_set<std::string> filterConfirmedTails(
      const std::vector<std::string>& tails) override;

 protected:
  virtual std::optional<nlohmann::json> post(const nlohmann::json& input) = 0;
};
}  // namespace iota
}  // namespace hub

#endif /* __HUB_IOTA_API_JSON_H_ */
