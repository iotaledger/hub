/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_CRYPTO_PROVIDER_BASE_H_
#define COMMON_CRYPTO_PROVIDER_BASE_H_

#include <string>

#include <nonstd/optional.hpp>
#include "common/crypto/types.h"

namespace common {
namespace crypto {

/// CryptoProvider abstract class.
/// Provides the cryptographic services necessary to
/// obtain new addresses based on salt and sign bundle hashes.
class CryptoProviderBase {
 public:
  /// Destructor
  virtual ~CryptoProviderBase() {}
  virtual nonstd::optional<common::crypto::Address> getAddressForUUID(
      const common::crypto::UUID& uuid) const = 0;

  /// Forces the calculation of a signature for a UUID and a bundle hash
  /// param[in] UUID - a UUID
  /// param[in] Hash - a bundleHash
  /// @return string - the signature
  nonstd::optional<std::string> forceGetSignatureForUUID(
      const common::crypto::UUID& uuid,
      const common::crypto::Hash& bundleHash) const {
    return doGetSignatureForUUID(uuid, bundleHash);
  }

  /// The desired security level
  virtual nonstd::optional<size_t> securityLevel(
      const common::crypto::UUID& uuid) const = 0;

  /// takes a 81 trytes address and calculates 9 trytes checksum on it
  /// param[in] address - the address on which checksum is calculated
  /// @return Checksum - the checksum of the address
  common::crypto::Checksum calcChecksum(std::string_view address) const;

  /// Checks if first 81 trytes checksums to last 9 of `addressWithChecksum`
  /// param[in] address - the address including the checksum - 90 trytes
  /// @return Address - the 81 trytes address if it validates
  nonstd::optional<common::crypto::Address> verifyAndStripChecksum(
      const std::string& address) const;

  /// Calculate the signature for a UUID and a bundle hash.
  /// param[in] UUID - a UUID
  /// param[in] Hash - a bundleHash
  /// @return string - the signature
  virtual nonstd::optional<std::string> getSignatureForUUID(
      const common::crypto::UUID& uuid,
      const common::crypto::Hash& bundleHash) const {
    return doGetSignatureForUUID(uuid, bundleHash);
  }

 protected:
  /// Calculate the signature for a UUID and a bundle hash
  /// param[in] UUID - a UUID
  /// param[in] Hash - a bundleHash
  /// @return string - the signature
  virtual nonstd::optional<std::string> doGetSignatureForUUID(
      const common::crypto::UUID& uuid,
      const common::crypto::Hash& bundleHash) const = 0;
};

}  // namespace crypto
}  // namespace common
#endif  // COMMON_CRYPTO_PROVIDER_BASE_H_
