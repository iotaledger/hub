/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_CRYPTO_ARGON2_PROVIDER_H_
#define HUB_CRYPTO_ARGON2_PROVIDER_H_

#include <cstdint>
#include <string>

#include "hub/crypto/provider.h"

namespace hub {
namespace crypto {

/// Argon2Provider class.
/// The argon2 provider provides the cryptographic services necessary to
/// obtain new addresses based on salt and sign bundle hashes.
class Argon2Provider : public CryptoProvider {
 public:
  Argon2Provider() = delete;
  /// Constructor
  /// param[in] salt - the salt that will be used in calculations
  explicit Argon2Provider(std::string salt);

  /// Get a new address for a given UUID and the salt
  /// param[in] UUID - a UUID
  common::crypto::Address getAddressForUUID(
      const common::crypto::UUID& uuid) const override;

  /// The current security level
  /// @return size_t - the security level (1 - 3)
  size_t securityLevel(const common::crypto::UUID& uuid) const override;

 protected:
  /// Calculate the signature for a UUID and a bundle hash
  /// param[in] UUID - a UUID
  /// param[in] Hash - a bundleHash
  /// @return string - the signature
  std::string doGetSignatureForUUID(
      const common::crypto::UUID& uuid,
      const common::crypto::Hash& bundleHash) const override;

 private:
  /// The salt that will be used all throughout the lifetime of the provider
  const std::string _salt;

  static constexpr uint32_t _argon_t_cost{1};
  static constexpr uint32_t _argon_m_cost{1 << 16};  // 64mebibytes
  static constexpr uint32_t _argon_parallelism{1};
};

}  // namespace crypto
}  // namespace hub
#endif  // HUB_CRYPTO_ARGON2_PROVIDER_H_
