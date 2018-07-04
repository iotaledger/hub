/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitcommon.com/iotaledger/rpccommon
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/stats/session.h"

namespace common {
std::ostream& operator<<(std::ostream& os, const ClientSession& session) {
  os << session.to_str();
  return os;
}
}  // namespace common
