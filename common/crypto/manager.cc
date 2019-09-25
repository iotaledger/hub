/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/crypto/manager.h"

namespace common {
namespace crypto {

CryptoManager& CryptoManager::get() {
  static CryptoManager instance;

  return instance;
}

}  // namespace crypto
}  // namespace common
