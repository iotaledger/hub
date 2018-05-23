/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */


#include "hub/stats/session.h"

namespace hub {
std::ostream& operator<<(std::ostream& os, const hub::ClientSession& session) {
  os << session.to_str();
  return os;
}
}  // namespace hub
