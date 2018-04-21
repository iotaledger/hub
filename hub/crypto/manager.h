#ifndef __HUB_CRYPTO_MANAGER_H_
#define __HUB_CRYPTO_MANAGER_H_

#include <memory>

#include "provider.h"


namespace hub {
namespace crypto {

class CryptoManager {
 private:
  explicit CryptoManager() {}

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
#endif /* __HUB_CRYPTO_MANAGER_H_ */
