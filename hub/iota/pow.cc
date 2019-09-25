/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/iota/pow.h"

namespace hub {
namespace iota {

POWManager& POWManager::get() {
  static POWManager instance;
  return instance;
}

nonstd::optional<cppclient::GetTransactionsToApproveResponse>
POWProvider::getAttachmentPoint(
    const nonstd::optional<std::string>& reference) const {
  return _api->getTransactionsToApprove(_depth, reference);
}

}  // namespace iota
}  // namespace hub
