// Copyright 2018 IOTA Foundation

#ifndef HUB_CRYPTO_TYPES_H_
#define HUB_CRYPTO_TYPES_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <sstream>
#include <string>
#include <string_view>

namespace hub {
namespace crypto {

class UUID {
 public:
  static constexpr auto BASE64_CHARS =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  static constexpr uint32_t UUID_SIZE = 64;
  static UUID generate();

  UUID();
  explicit UUID(const std::string& str);

  operator const char*() const;

  std::string toString() const;

  const std::array<uint8_t, UUID_SIZE>& data() const;

 private:
  std::array<uint8_t, UUID_SIZE> _data;
};

bool operator==(hub::crypto::UUID lhs, hub::crypto::UUID rhs);
bool operator!=(hub::crypto::UUID lhs, hub::crypto::UUID rhs);

static constexpr auto TRYTE_CHARS = "9ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static constexpr uint8_t MIN_TRYTE = 'A';
static constexpr uint8_t MAX_TRYTE = 'Z';
static constexpr uint8_t NULL_TRYTE = '9';

template <std::size_t N>
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

  explicit TryteArray(const std::string& data) {
    validateSize(data.size());

    if (!std::all_of(data.begin(), data.end(), [](char c) {
          return ((c >= MIN_TRYTE && c <= MAX_TRYTE) || c == NULL_TRYTE);
        })) {
      throw std::runtime_error(__FUNCTION__ +
                               std::string(" initialization string: ") + data +
                               " contains invalid characters");
    }

    for (uint32_t i = 0; i < N; ++i) {
      _data[i] = data[i];
    }
  }

  explicit TryteArray(const char* data) : TryteArray(std::string(data)) {}

  const std::array<uint8_t, N>& data() { return _data; }

  std::string toString() const {
    std::stringstream ss;
    for (auto i : _data) {
      ss << i;
    }
    return ss.str();
  }

  const std::array<uint8_t, N>& data() const { return _data; }

  uint32_t size() const { return _data.size(); }

 private:
  std::array<uint8_t, N> _data;
};

template <std::size_t N>
bool operator==(hub::crypto::TryteArray<N> lhs,
                hub::crypto::TryteArray<N> rhs) {
  return lhs.data() == rhs.data();
}
template <std::size_t N>
bool operator!=(hub::crypto::TryteArray<N> lhs,
                hub::crypto::TryteArray<N> rhs) {
  return !(lhs == rhs);
}

struct HashTag {
  static constexpr uint32_t SIZE = 81;
};
struct AddressTag {
  static constexpr uint32_t SIZE = 81;
};

template <typename Tag>
class TryteArrayType {};

template <>
class TryteArrayType<HashTag> {
 public:
  const TryteArray<HashTag::SIZE>& data() { return _data; }
  uint32_t size() const { return _data.size(); }
  std::string toString() const { return _data.toString(); }
  explicit TryteArrayType(const std::string& data) : _data(data) {}

  explicit TryteArrayType(const char* data) : _data(std::string(data)) {}

 private:
  TryteArray<HashTag::SIZE> _data;
};
template <>
class TryteArrayType<AddressTag> {
 public:
  const TryteArray<AddressTag::SIZE>& data() { return _data; }
  uint32_t size() const { return _data.size(); }
  std::string toString() const { return _data.toString(); }
  explicit TryteArrayType(const std::string& data) : _data(data) {}

  explicit TryteArrayType(const char* data) : _data(std::string(data)) {}

 private:
  TryteArray<AddressTag::SIZE> _data;
};

template <typename Tag>
bool operator==(hub::crypto::TryteArrayType<Tag> lhs,
                hub::crypto::TryteArrayType<Tag> rhs) {
  return lhs.data() == rhs.data();
}
template <typename Tag>
bool operator!=(hub::crypto::TryteArrayType<Tag> lhs,
                hub::crypto::TryteArrayType<Tag> rhs) {
  return !(lhs == rhs);
}

using Hash = TryteArrayType<HashTag>;
using Address = TryteArrayType<AddressTag>;

};  // namespace crypto
}  // namespace hub

#endif  // HUB_CRYPTO_TYPES_H_
