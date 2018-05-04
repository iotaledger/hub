// Copyright 2018 IOTA Foundation

#include "hub/crypto/manager.h"

namespace hub {
namespace crypto {

CryptoManager& CryptoManager::get() {
  static CryptoManager instance;

  return instance;
}

}  // namespace crypto
}  // namespace hub
