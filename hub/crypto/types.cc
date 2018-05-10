// Copyright 2018 IOTA Foundation

#include "hub/crypto/types.h"

#include <cstring>
#include <random>
#include <utility>

namespace hub {
namespace crypto {

UUID UUID::generate() {
  std::string res;
  std::random_device rd;
  std::uniform_int_distribution<> index_dist(0, UUID_SIZE - 1);

  for (uint32_t i = 0; i < UUID_SIZE; ++i) {
    res += BASE64_CHARS[index_dist(rd)];
  }
  return UUID(res);
}

UUID::UUID() { _data = std::move(UUID::generate()).data(); }

UUID::UUID(const std::string& str) {
  std::stringstream ss(str);
  for (auto& i : _data) {
    ss >> i;
  }
}

std::string UUID::toString() const {
  std::stringstream ss;
  for (auto i : _data) {
    ss << i;
  }
  return ss.str();
}

UUID::operator const char*() const { return toString().c_str(); }

const std::array<uint8_t, hub::crypto::UUID::UUID_SIZE>& UUID::data() const {
  return _data;
}

bool operator==(hub::crypto::UUID lhs, hub::crypto::UUID rhs) {
  return lhs.data() == rhs.data();
}

bool operator!=(hub::crypto::UUID lhs, hub::crypto::UUID rhs) {
  return !(lhs == rhs);
}
}  // namespace crypto
}  // namespace hub
