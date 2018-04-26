// Copyright 2018 IOTA Foundation

#ifndef HUB_IOTA_BEAST_H_
#define HUB_IOTA_BEAST_H_

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json_fwd.hpp>

#include "hub/iota/api.h"
#include "hub/iota/api_json.h"

namespace hub {
namespace iota {

class BeastIotaAPI : virtual public IotaAPI, public IotaJsonAPI {
 public:
  BeastIotaAPI() = delete;
  BeastIotaAPI(std::string host, uint32_t port)
      : _host(std::move(host)), _port(port) {}
  virtual ~BeastIotaAPI() {}

 protected:
  std::optional<nlohmann::json> post(const nlohmann::json& input) override;

 private:
  const std::string _host;
  const uint32_t _port;
};
}  // namespace iota
}  // namespace hub

#endif  // HUB_IOTA_BEAST_H_
