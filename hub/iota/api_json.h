#ifndef __HUB_IOTA_API_MIXIN_H_
#define __HUB_IOTA_API_MIXIN_H_

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "api.h"
#include "json.h"

namespace hub {
namespace iota {

class IotaJsonAPI : public IotaAPI {
 public:
  virtual std::unordered_map<std::string, uint64_t> getBalances(
      const std::vector<std::string> addresses) override;

  virtual std::vector<Bundle> getConfirmedBundlesForAddress(
      const std::string& address) override;

  virtual std::unordered_set<std::string> filterConfirmedTails(
      const std::vector<std::string> tails) override;

 protected:
  virtual std::optional<nlohmann::json> post(const nlohmann::json& input) = 0;
};
}  // namespace iota
}  // namespace hub

#endif /* __HUB_IOTA_API_MIXIN_H_ */
