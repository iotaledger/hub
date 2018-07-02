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
  /// @throws sqlpp::exception if UUID was already used for a signature
  /// @return string - the signature
  std::string getSignatureForUUID(hub::db::Connection& connection,
                                  const UUID& uuid,
                                  const Hash& bundleHash) const {
    connection.markUUIDAsSigned(uuid);

    return doGetSignatureForUUID(uuid, bundleHash);
  }

  /// Forces the calculation of a signature for a UUID and a bundle hash
  /// param[in] UUID - a UUID
  /// param[in] Hash - a bundleHash
  /// @return string - the signature
  std::string forceGetSignatureForUUID(const UUID& uuid,
                                       const Hash& bundleHash) const {
    return doGetSignatureForUUID(uuid, bundleHash);
  }

  /// The desired security level
  virtual size_t securityLevel() const = 0;

  /// takes a 81 trytes address and calculates 9 trytes checksum on it
  /// param[in] address - the address on which checksum is calculated
  /// @return Checksum - the checksum of the address
  Checksum calcChecksum(std::string_view address) const;

  /// Checks if first 81 trytes checksums to last 9 of `addressWithChecksum`
  /// param[in] address - the address including the checksum - 90 trytes
  /// @return Address - the 81 trytes address if it validates
  nonstd::optional<Address> verifyAndStripChecksum(
      const std::string& address) const;

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
