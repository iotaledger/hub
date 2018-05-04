// Copyright 2018 IOTA Foundation

#ifndef HUB_IOTA_REMOTE_POW_H_
#define HUB_IOTA_REMOTE_POW_H_

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "hub/iota/api.h"
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
