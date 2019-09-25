/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_IOTA_REMOTE_POW_H_
#define HUB_IOTA_REMOTE_POW_H_

#include <memory>
#include <nonstd/optional.hpp>
#include <string>
#include <utility>
#include <vector>

#include "cppclient/api.h"
#include "hub/iota/pow.h"

namespace hub {
namespace iota {

class RemotePOW : public POWProvider {
 public:
  using POWProvider::POWProvider;

 protected:
  std::vector<std::string> doPOW(const std::vector<std::string>& trytes,
                                 const std::string& trunk,
                                 const std::string& branch) const override {
    return _api->attachToTangle(trunk, branch, mwm(), trytes);
  }
};
}  // namespace iota
}  // namespace hub

#endif  // HUB_IOTA_REMOTE_POW_H_
