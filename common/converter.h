/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_CONVERTER_H_
#define COMMON_CONVERTER_H_

#include <string>
#include <string_view>
#include <chrono>

namespace common {
    std::string boolToString(bool isTrue);
    bool stringToBool(std::string_view isTrueStr);

    uint64_t timepointToUint64(std::chrono::system_clock::time_point timepoint);
}

#endif  // COMMON_COMMON_H_
