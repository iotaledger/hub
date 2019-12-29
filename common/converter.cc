/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/converter.h"

namespace common {

std::string boolToString(bool isTrue) { return isTrue ? "true" : "false"; }
bool stringToBool(std::string_view isTrueStr) {
  return (isTrueStr.compare("true") == 0) ? true : false;
}

uint64_t timepointToUint64(std::chrono::system_clock::time_point timepoint) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             timepoint.time_since_epoch())
      .count();
}
}  // namespace common
