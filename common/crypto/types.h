/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_TYPES_H_
#define COMMON_TYPES_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <functional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace common {
namespace crypto {

/// UUID class.
/// Provides methods to work with UUIDs
class UUID {
 public:
  static constexpr auto BASE64_CHARS =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  static constexpr uint32_t UUID_SIZE = 64;

  /// Constructor
  UUID();
  /// Constructor. Creates a new UUID from a string.
  /// @param[in] sv - string representation
  explicit UUID(const std::string_view& sv);

  /// Destructor. Zeroes array upon destruction.
  ~UUID() { _data.fill(0); }

  /// String representation of UUID
  /// @return string - string representation
  std::string str() const;

  /// String view representation of UUID
  /// @return string - string representation
  std::string_view str_view() const;

  /// Binary representation of UUID
  /// @return std::array - binary representation
  const std::array<uint8_t, UUID_SIZE>& data() const;

 private:
  /// Generate a new UUID
  /// @return std::array - binary representation
  static std::array<uint8_t, UUID_SIZE> generate();

  /// Generate a new UUID from string
  /// @param[in] sv - a string view
  /// @return std::array - binary representation
  static std::array<uint8_t, UUID_SIZE> fromStringView(
      const std::string_view& sv);

  /// Internal representation of UUID
  std::array<uint8_t, UUID_SIZE> _data;
};

bool operator==(const common::crypto::UUID& lhs,
                const common::crypto::UUID& rhs);
bool operator!=(const common::crypto::UUID& lhs,
                const common::crypto::UUID& rhs);

static constexpr auto TRYTE_CHARS = "9ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static constexpr uint8_t MIN_TRYTE = 'A';
static constexpr uint8_t MAX_TRYTE = 'Z';
static constexpr uint8_t NULL_TRYTE = '9';

/// Template for array of trytes class.
/// Provides methods to work with tryte arrays
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

  /// Constructor
  /// @param[in] data - a std::string_view
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

  /// Constructor
  /// @param[in] data - a standard C string
  explicit TryteArray(const char* data) : TryteArray(std::string(data)) {}

  const std::array<uint8_t, N>& data() const { return _data; }

  std::string str() const { return std::string(str_view()); }

  /// @return std::string_view - a std::string_view representation of the trytes
  std::string_view str_view() const {
    return std::string_view(reinterpret_cast<const char*>(_data.data()), N);
  }

  /// @return uint32_t - The number of bytes used to represent the trytes
  uint32_t size() const { return _data.size(); }

  static constexpr std::size_t length() { return N; }

  bool isNull() {
    return std::all_of(_data.begin(), _data.end(),
                       [](char c) { return (c == NULL_TRYTE); });
  }

 private:
  /// Internal representation of the trytes
  std::array<uint8_t, N> _data;
};

template <std::size_t N, typename TAG>
bool operator==(const common::crypto::TryteArray<N, TAG>& lhs,
                const common::crypto::TryteArray<N, TAG>& rhs) {
  return lhs.data() == rhs.data();
}
template <std::size_t N, typename TAG>
bool operator!=(const common::crypto::TryteArray<N, TAG>& lhs,
                const common::crypto::TryteArray<N, TAG>& rhs) {
  return !(lhs == rhs);
}

struct HashTag {};
struct AddressTag {};
struct TagTag {};
struct ChecksumTag {};
struct MessageTag {};

using Checksum = TryteArray<9, ChecksumTag>;
using Hash = TryteArray<81, HashTag>;
using Address = TryteArray<81, AddressTag>;
using Tag = TryteArray<27, TagTag>;
using Message = TryteArray<2187, MessageTag>;

};  // namespace crypto
}  // namespace common

namespace std {

template <std::size_t N, typename TAG>
struct hash<common::crypto::TryteArray<N, TAG>> {
 public:
  std::size_t operator()(const common::crypto::TryteArray<N, TAG>& k) const {
    return std::hash<std::string_view>()(k.str_view());
  }
};

template <>
struct hash<common::crypto::UUID> {
 public:
  std::size_t operator()(const common::crypto::UUID& k) const {
    return std::hash<std::string_view>()(k.str_view());
  }
};
}  // namespace std

#endif  // COMMON_TYPES_H_
