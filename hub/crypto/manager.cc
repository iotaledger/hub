#include "manager.h"

namespace hub {
namespace crypto {

CryptoManager& CryptoManager::get() {
  static CryptoManager instance;

  return instance;
}

CryptoProvider& CryptoManager::provider() {
  return *_provider;
}

}  // namespace crypto
}  // namespace hub
