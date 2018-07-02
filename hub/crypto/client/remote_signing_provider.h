/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_CRYPTO_CLIENT_H_
#define HUB_CRYPTO_CLIENT_H_

#include <string>
#include "hub/crypto/provider.h"

#include "proto/signing_server.grpc.pb.h"
#include "proto/signing_server.pb.h"

namespace hub {
namespace crypto {

class RemoteSigningProvider : public CryptoProvider {
 public:
  /// Constructor
  /// param[in] url - the url for the grpc service
  explicit RemoteSigningProvider(const std::string& url,
                                 const std::string& authMode,
                                 const std::string& certPath,
                                 const std::string& chainPath,
                                 const std::string& keyPath);

  /// Get a new address for a given UUID and the salt
  /// param[in] UUID - a UUID
  Address getAddressForUUID(const UUID& uuid) const override;

  /// The current security level
  /// @return size_t - the security level (1 - 3)
  size_t securityLevel(const UUID& uuid) const override;

 protected:
  /// Calculate the signature for a UUID and a bundle hash
  /// param[in] UUID - a UUID
  /// param[in] Hash - a bundleHash
  /// @return string - the signature
  std::string doGetSignatureForUUID(const UUID& uuid,
                                    const Hash& bundleHash) const override;

 private:
  std::unique_ptr<signing::rpc::SigningServer::Stub> _stub;
};

}  // namespace crypto
}  // namespace hub
#endif  // HUB_CRYPTO_CLIENT_H_
