/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

/// \file crypto/types.h
/// Helpers that provide functionality to deal with UUIDs, trytes and hashes
///

#ifndef HUB_CRYPTO_TYPES_H_
#define HUB_CRYPTO_TYPES_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <functional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace hub {
namespace crypto {

class UUID {
 public:
  static constexpr auto BASE64_CHARS =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  static constexpr uint32_t UUID_SIZE = 64;

  UUID();
  explicit UUID(const std::string_view& sv);

  std::string str() const;

  std::string_view str_view() const;

  const std::array<uint8_t, UUID_SIZE>& data() const;

 private:
  static std::array<uint8_t, UUID_SIZE> generate();
  static std::array<uint8_t, UUID_SIZE> fromStringView(
      const std::string_view& sv);
  std::array<uint8_t, UUID_SIZE> _data;
};

bool operator==(const hub::crypto::UUID& lhs, const hub::crypto::UUID& rhs);
bool operator!=(const hub::crypto::UUID& lhs, const hub::crypto::UUID& rhs);

static constexpr auto TRYTE_CHARS = "9ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static constexpr uint8_t MIN_TRYTE = 'A';
static constexpr uint8_t MAX_TRYTE = 'Z';
static constexpr uint8_t NULL_TRYTE = '9';

template <std::size_t N, typename TAG>
class TryteArray {
 public:
  void validateSize(std::size_t size) {
    if (size != N) {
      throw std::runtime_error(
          __FUNCTION__ +
          std::string("initialization string has wrong size (expected size: ") +
          std::to_string(N) + " observed size: " + std::to_string(size) + ")");
    }
  }

  explicit TryteArray(const std::string_view& data) {
    validateSize(data.size());

    if (!std::all_of(data.begin(), data.end(), [](char c) {
          return ((c >= MIN_TRYTE && c <= MAX_TRYTE) || c == NULL_TRYTE);
        })) {
      throw std::runtime_error(__FUNCTION__ +
                               std::string(" initialization string: ") +
                               data.data() + " contains invalid characters");
    }

    std::copy(std::begin(data), std::end(data), std::begin(_data));
  }

  explicit TryteArray(const char* data) : TryteArray(std::string(data)) {}

  const std::array<uint8_t, N>& data() const { return _data; }

  std::string str() const { return std::string(str_view()); }

  std::string_view str_view() const {
    return std::string_view(reinterpret_cast<const char*>(_data.data()), N);
  }

  uint32_t size() const { return _data.size(); }

 private:
  std::array<uint8_t, N> _data;
};

template <std::size_t N, typename TAG>
bool operator==(const hub::crypto::TryteArray<N, TAG>& lhs,
                const hub::crypto::TryteArray<N, TAG>& rhs) {
  return lhs.data() == rhs.data();
}
template <std::size_t N, typename TAG>
bool operator!=(const hub::crypto::TryteArray<N, TAG>& lhs,
                const hub::crypto::TryteArray<N, TAG>& rhs) {
  return !(lhs == rhs);
}

struct HashTag {};
struct AddressTag {};

using Hash = TryteArray<81, HashTag>;
using Address = TryteArray<81, AddressTag>;

};  // namespace crypto
}  // namespace hub

namespace std {

template <std::size_t N, typename TAG>
struct hash<hub::crypto::TryteArray<N, TAG>> {
 public:
  std::size_t operator()(const hub::crypto::TryteArray<N, TAG>& k) const {
    return std::hash<std::string_view>()(k.str_view());
  }
};

template <>
struct hash<hub::crypto::UUID> {
 public:
  std::size_t operator()(const hub::crypto::UUID& k) const {
    return std::hash<std::string_view>()(k.str_view());
  }
};
}  // namespace std

#endif  // HUB_CRYPTO_TYPES_H_
