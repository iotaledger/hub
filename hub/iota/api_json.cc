#include <cstdint>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <glog/logging.h>
#include <boost/range/combine.hpp>

#include "api_json.h"
#include "json.h"

using json = nlohmann::json;

namespace hub {
namespace iota {

std::unordered_map<std::string, uint64_t> IotaJsonAPI::getBalances(
    const std::vector<std::string>& addresses) {
  json req;
  req["command"] = "getBalances";
  req["addresses"] = addresses;
  req["threshold"] = 100;

  auto maybeResponse = post(std::move(req));

  if (!maybeResponse) {
    LOG(INFO) << __FUNCTION__ << " request failed.";
    return {};
  }

  auto response = maybeResponse.value();
  auto balances = response["balances"].get<std::vector<std::string>>();

  std::unordered_map<std::string, uint64_t> result;

  for (const auto& tup : boost::combine(addresses, balances)) {
    std::string add, bal;
    boost::tie(add, bal) = tup;
    result.emplace(std::move(add), std::stoull(bal));
  }
  LOG(ERROR) << "baba";

  return result;
}

std::vector<Bundle> IotaJsonAPI::getConfirmedBundlesForAddress(
    const std::string& address) {
  return {};
}

std::unordered_set<std::string> IotaJsonAPI::filterConfirmedTails(
    const std::vector<std::string>& tails) {
  return {};
}

}  // namespace iota
}  // namespace hub
