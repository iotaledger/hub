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

 public:
  static CryptoManager& get();
  CryptoProvider& provider();

  void setProvider(std::unique_ptr<CryptoProvider> provider) {
    _provider = std::move(provider);
  }

 private:
  std::unique_ptr<CryptoProvider> _provider;
};

}  // namespace crypto
}  // namespace hub
#endif  // HUB_CRYPTO_MANAGER_H_
