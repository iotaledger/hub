/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_CRYPTO_REMOTE_SIGNING_PROVIDER_H_
#define HUB_CRYPTO_REMOTE_SIGNING_PROVIDER_H_

#include <memory>
#include <string>

#include <nonstd/optional.hpp>

#include "common/crypto/provider_base.h"
#include "hub/db/db.h"
#include "proto/signing_server.grpc.pb.h"
#include "proto/signing_server.pb.h"

namespace hub {
namespace crypto {

class RemoteSigningProvider : public common::crypto::CryptoProviderBase {
 public:
  /// Constructor
  /// param[in] url - the url for the grpc service
  explicit RemoteSigningProvider(const std::string& url,
                                 const std::string& certPath,
                                 const std::string& chainPath,
                                 const std::string& keyPath);

  /// Get a new address for a given UUID and the salt
  /// param[in] UUID - a UUID
  nonstd::optional<common::crypto::Address> getAddressForUUID(
      const common::crypto::UUID& uuid) const override;

  /// The current security level
  /// @return size_t - the security level (1 - 3)
  nonstd::optional<size_t> securityLevel(
      const common::crypto::UUID& uuid) const override;

  nonstd::optional<std::string> getSignatureForUUID(
      const common::crypto::UUID& uuid,
      const common::crypto::Hash& bundleHash) const override {
    auto& connection = hub::db::DBManager::get().connection();
    connection.markUUIDAsSigned(uuid);

    return doGetSignatureForUUID(uuid, bundleHash);
  }

 protected:
  /// Calculate the signature for a UUID and a bundle hash
  /// param[in] UUID - a UUID
  /// param[in] Hash - a bundleHash
  /// @return string - the signature
  nonstd::optional<std::string> doGetSignatureForUUID(
      const common::crypto::UUID& uuid,
      const common::crypto::Hash& bundleHash) const override;

 private:
  std::unique_ptr<signing::rpc::SigningServer::Stub> _stub;
};

}  // namespace crypto
}  // namespace hub
#endif  // HUB_CRYPTO_REMOTE_SIGNING_PROVIDER_H_
