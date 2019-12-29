/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_IOTA_POW_H_
#define HUB_IOTA_POW_H_

#include <glog/logging.h>
#include <memory>
#include <nonstd/optional.hpp>
#include <string>
#include <utility>
#include <vector>

#include "cppclient/api.h"

namespace hub {
namespace iota {

/// POWProvider (Proof Of Work provider)
class POWProvider {
 public:
  /// Constructor
  POWProvider(std::shared_ptr<cppclient::IotaAPI> api, size_t depth, size_t mwm)
      : _api(std::move(api)), _depth(depth), _mwm(mwm) {}

  /// Destructor
  virtual ~POWProvider() {}

  size_t mwm() const { return _mwm; }
  size_t depth() const { return _depth; }

  virtual nonstd::optional<cppclient::GetTransactionsToApproveResponse>
  getAttachmentPoint(const nonstd::optional<std::string>& reference) const;

  virtual std::vector<std::string> performPOW(
      const std::vector<std::string>& trytes) const {
    auto maybeLocation = getAttachmentPoint({});
    if (!maybeLocation) {
      LOG(ERROR) << "Failed to get attachment point";
      return {};
    }

    auto location = maybeLocation.value();
    return doPOW(trytes, location.trunkTransaction, location.branchTransaction);
  }

  virtual std::vector<std::string> doPOW(const std::vector<std::string>& trytes,
                                         const std::string& trunk,
                                         const std::string& branch) const = 0;

 protected:
  const std::shared_ptr<cppclient::IotaAPI> _api;
  const size_t _depth;
  const size_t _mwm;
};

class POWManager {
 private:
  POWManager() = default;

 public:
  static POWManager& get();

  POWProvider& provider() { return *_provider; }

  void setProvider(std::unique_ptr<POWProvider> provider) {
    _provider = std::move(provider);
  }

 private:
  std::unique_ptr<POWProvider> _provider;
};

}  // namespace iota
}  // namespace hub

#endif  // HUB_IOTA_POW_H_
