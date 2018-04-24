#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "api_json.h"
#include "json.h"

namespace hub {
namespace iota {

std::unordered_map<std::string, uint64_t> IotaJsonAPI::getBalances(
    const std::vector<std::string> addresses) {
  return {};
}

std::vector<Bundle> IotaJsonAPI::getConfirmedBundlesForAddress(
    const std::string& address) {
  return {};
}

std::unordered_set<std::string> IotaJsonAPI::filterConfirmedTails(
    const std::vector<std::string> tails) {
  return {};
}

}  // namespace iota
}  // namespace hub
