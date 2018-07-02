/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_CRYPTO_MANAGER_H_
#define HUB_CRYPTO_MANAGER_H_

#include <memory>
#include <utility>

#include "hub/crypto/provider.h"

namespace hub {
namespace crypto {

/// CryptoManager holds the single instance of a CryptoProvider
class CryptoManager {
 private:
  /// Constructor
  CryptoManager() {}
  /// deleted
  CryptoManager(CryptoManager const&) = delete;
  /// deleted
  CryptoManager(CryptoManager&&) = delete;
  /// deleted
  CryptoManager& operator=(CryptoManager const&) = delete;
  /// deleted
  CryptoManager& operator=(CryptoManager&&) = delete;

 public:
  /// Get the singleton instance
  /// @return CryptoManager - the singleton instance of CryptoManager
  static CryptoManager& get();

  /// Set the cryptography provider
  /// @param[in] provider - an instance of CryptoProvider
  void setProvider(std::unique_ptr<CryptoProvider> provider) {
    _provider = std::move(provider);
  }

  /// Get the cryptography provider
  /// @return CryptoProvider - an instance of CryptoProvider
  CryptoProvider& provider() { return *_provider; }

 private:
  /// The cryptography provider
  std::unique_ptr<CryptoProvider> _provider;
};

}  // namespace crypto
}  // namespace hub
#endif  // HUB_CRYPTO_MANAGER_H_
