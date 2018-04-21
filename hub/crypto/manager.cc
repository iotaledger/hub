#include "manager.h"

namespace iota {
namespace crypto {

CryptoManager& CryptoManager::get() {
  static CryptoManager instance;

  return instance;
}

CryptoProvider& CryptoManager::provider() {
  return *_provider;
}

}  // namespace crypto
}  // namespace iota
