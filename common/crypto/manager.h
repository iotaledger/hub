/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_CRYPTO_MANAGER_H_
#define COMMON_CRYPTO_MANAGER_H_

#include <memory>
#include <utility>

#include "common/crypto/provider_base.h"

namespace common {
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
  void setProvider(std::unique_ptr<CryptoProviderBase> provider) {
    _provider = std::move(provider);
  }

  /// Get the cryptography provider
  /// @return CryptoProvider - an instance of CryptoProvider
  CryptoProviderBase& provider() { return *_provider; }

 private:
  /// The cryptography provider
  std::unique_ptr<CryptoProviderBase> _provider;
};

}  // namespace crypto
}  // namespace common
#endif  // COMMON_CRYPTO_MANAGER_H_
