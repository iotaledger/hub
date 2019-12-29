/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_CRYPTO_LOCAL_PROVIDER_H_
#define HUB_CRYPTO_LOCAL_PROVIDER_H_

#include <string>

#include "common/crypto/argon2_provider.h"
#include "common/crypto/types.h"
#include "hub/db/helper.h"

namespace hub {
namespace crypto {

/// LocalSigningProvider class.
/// Provides the cryptographic services necessary to
/// obtain new addresses based on salt and sign bundle hashes.
class LocalSigningProvider : public common::crypto::Argon2Provider {
 public:
  /// Calculate the signature for a UUID and a bundle hash
  /// param[in] connection - connection to the local database
  /// param[in] UUID - a UUID
  /// param[in] Hash - a bundleHash
  /// @throws sqlpp::exception if UUID was already used for a signature
  /// @return string - the signature
  nonstd::optional<std::string> getSignatureForUUID(
      const common::crypto::UUID& uuid,
      const common::crypto::Hash& bundleHash) const override;

  explicit LocalSigningProvider(const std::string& salt)
      : Argon2Provider(salt) {}
};

}  // namespace crypto
}  // namespace hub
#endif  // HUB_CRYPTO_LOCAL_PROVIDER_H_
