/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */


#ifndef HUB_CRYPTO_PROVIDER_H_
#define HUB_CRYPTO_PROVIDER_H_

#include <string>

#include "hub/crypto/types.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"

namespace hub {
namespace crypto {

/// CryptoProvider abstract class.
/// Provides the cryptographic services necessary to
/// obtain new addresses based on salt and sign bundle hashes.
class CryptoProvider {
 public:
  /// Destructor
  virtual ~CryptoProvider() {}
  virtual Address getAddressForUUID(const UUID& uuid) const = 0;

  /// Calculate the signature for a UUID and a bundle hash
  /// param[in] connection - connection to the local database
  /// param[in] UUID - a UUID
  /// param[in] Hash - a bundleHash
  /// @return string - the signature
  std::string getSignatureForUUID(hub::db::Connection& connection,
                                  const UUID& uuid,
                                  const Hash& bundleHash) const {
    connection.markUUIDAsSigned(uuid);

    return doGetSignatureForUUID(uuid, bundleHash);
  }

  /// The desired security level
  virtual size_t securityLevel() const = 0;

 protected:
  /// Calculate the signature for a UUID and a bundle hash.
  /// param[in] UUID - a UUID
  /// param[in] Hash - a bundleHash
  /// @return string - the signature
  virtual std::string doGetSignatureForUUID(const UUID& uuid,
                                            const Hash& bundleHash) const = 0;
};

}  // namespace crypto
}  // namespace hub
#endif  // HUB_CRYPTO_PROVIDER_H_
