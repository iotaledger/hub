#ifndef __HUB_IOTA_BEAST_H_
#define __HUB_IOTA_BEAST_H_

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "api.h"
#include "json.h"

namespace hub {
namespace iota {

class BeastIotaAPI : public IotaAPI {
 public:
  BeastIotaAPI() = delete;

  BeastIotaAPI(std::string host, uint32_t port)
      : _host(std::move(host)), _port(port) {}

  virtual std::unordered_map<std::string, uint64_t> getBalances(
      const std::vector<std::string> addresses) override {
    return {};
  }

  virtual std::vector<Bundle> getConfirmedBundlesForAddress(
      const std::string& address) override {
    return {};
  }

  virtual std::unordered_set<std::string> filterConfirmedTails(
      const std::vector<std::string> tails) override {
    return {};
  }

 protected:
  virtual std::optional<nlohmann::json> post(const nlohmann::json& input);

 private:
  const std::string _host;
  const uint32_t _port;
};
}  // namespace iota
}  // namespace hub

#endif /* __HUB_IOTA_BEAST_H_ */
