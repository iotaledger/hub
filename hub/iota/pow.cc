// Copyright 2018 IOTA Foundation

#include "hub/iota/pow.h"
#include "hub/iota/api.h"

namespace hub {
namespace iota {

POWManager& POWManager::get() {
  static POWManager instance;
  return instance;
}

std::pair<std::string, std::string> POWProvider::getAttachmentPoint(
    const nonstd::optional<std::string>& reference) const {
  return _api->getTransactionsToApprove(_depth, reference);
}

}  // namespace iota
}  // namespace hub
