/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_IOTA_LOCAL_POW_H_
#define HUB_IOTA_LOCAL_POW_H_

#include <memory>
#include <nonstd/optional.hpp>
#include <string>
#include <utility>
#include <vector>

#include "hub/iota/pow.h"

namespace hub {
namespace iota {

class LocalPOW : public POWProvider {
 public:
  using POWProvider::POWProvider;

  LocalPOW(std::shared_ptr<cppclient::IotaAPI> api, size_t depth, size_t mwm);

  static constexpr uint16_t OBSOLETE_TAG_OFFSET = 2295;
  static constexpr uint16_t TIMESTAMP_OFFSET = 2322;
  static constexpr uint16_t TRUNK_OFFSET = 2430;
  static constexpr uint16_t BRANCH_OFFSET = 2511;
  static constexpr uint16_t TAG_OFFSET = 2592;
  static constexpr uint16_t NONCE_OFFSET = 2646;

 protected:
  std::vector<std::string> doPOW(const std::vector<std::string>& trytes,
                                 const std::string& trunk,
                                 const std::string& branch) const override;
};
}  // namespace iota
}  // namespace hub

#endif  // HUB_IOTA_LOCAL_POW_H_
