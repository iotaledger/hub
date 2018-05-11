// Copyright 2018 IOTA Foundation

#include "hub/crypto/types.h"

#include <cstring>
#include <random>
#include <utility>

namespace hub {
namespace crypto {

std::array<uint8_t, UUID::UUID_SIZE> UUID::generate() {
  std::array<uint8_t, UUID_SIZE> res;
  std::random_device rd;
  std::uniform_int_distribution<> index_dist(0, UUID_SIZE - 1);

  for (uint32_t i = 0; i < UUID_SIZE; ++i) {
    res[i] = BASE64_CHARS[index_dist(rd)];
  }
  return res;
}

UUID::UUID() : _data(std::move(UUID::generate())) {}

UUID::UUID(const std::string_view& sv) {
  std::copy(std::begin(sv), std::end(sv), std::begin(_data));
}

std::string UUID::str() const { return std::string(strView()); }

std::string_view UUID::strView() const {
  return std::string_view(reinterpret_cast<const char*>(_data.data()),
                          UUID_SIZE);
}
UUID::operator const char*() const { return strView().data(); }

const std::array<uint8_t, hub::crypto::UUID::UUID_SIZE>& UUID::data() const {
  return _data;
}

bool operator==(const hub::crypto::UUID& lhs, const hub::crypto::UUID& rhs) {
  return lhs.data() == rhs.data();
}

bool operator!=(const hub::crypto::UUID& lhs, const hub::crypto::UUID& rhs) {
  return !(lhs == rhs);
}
}  // namespace crypto
}  // namespace hub
