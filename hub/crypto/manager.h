// Copyright 2018 IOTA Foundation

#ifndef HUB_CRYPTO_MANAGER_H_
#define HUB_CRYPTO_MANAGER_H_

#include <memory>
#include <utility>

#include "hub/crypto/provider.h"

namespace hub {
namespace crypto {

class CryptoManager {
 private:
  CryptoManager() {}
  CryptoManager(CryptoManager const&) = delete;
  CryptoManager(CryptoManager&&) = delete;
  CryptoManager& operator=(CryptoManager const&) = delete;
  CryptoManager& operator=(CryptoManager&&) = delete;

 public:
  static CryptoManager& get();

  void setProvider(std::unique_ptr<CryptoProvider> provider) {
    _provider = std::move(provider);
  }

  CryptoProvider& provider() { return *_provider; }

 private:
  std::unique_ptr<CryptoProvider> _provider;
};

}  // namespace crypto
}  // namespace hub
#endif  // HUB_CRYPTO_MANAGER_H_
