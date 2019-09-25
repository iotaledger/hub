/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/crypto/types.h"

#include <glog/logging.h>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <cstring>
#include <utility>

namespace common {
namespace crypto {

std::array<uint8_t, UUID::UUID_SIZE> UUID::generate() {
  std::array<uint8_t, UUID_SIZE> res;
  boost::random::random_device rd;
  boost::random::uniform_int_distribution<> index_dist(0, UUID_SIZE - 1);
  for (uint32_t i = 0; i < UUID_SIZE; ++i) {
    res[i] = BASE64_CHARS[index_dist(rd)];
  }
  return res;
}

std::array<uint8_t, UUID::UUID_SIZE> UUID::fromStringView(
    const std::string_view& sv) {
  std::array<uint8_t, UUID_SIZE> res;
  std::copy(std::begin(sv), std::end(sv), std::begin(res));
  return res;
}

UUID::UUID() {
  try {
    _data = std::move(UUID::generate());
  } catch (const std::exception& ex) {
    LOG(FATAL) << " Failed in generating UUID: " << ex.what();
  }
}

UUID::UUID(const std::string_view& sv) : _data(fromStringView(sv)) {}

std::string UUID::str() const { return std::string(str_view()); }

std::string_view UUID::str_view() const {
  return std::string_view(reinterpret_cast<const char*>(_data.data()),
                          UUID_SIZE);
}

const std::array<uint8_t, common::crypto::UUID::UUID_SIZE>& UUID::data() const {
  return _data;
}

bool operator==(const common::crypto::UUID& lhs,
                const common::crypto::UUID& rhs) {
  return lhs.data() == rhs.data();
}

bool operator!=(const common::crypto::UUID& lhs,
                const common::crypto::UUID& rhs) {
  return !(lhs == rhs);
}
}  // namespace crypto
}  // namespace common
