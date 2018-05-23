/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */


#include "hub/crypto/manager.h"

namespace hub {
namespace crypto {

CryptoManager& CryptoManager::get() {
  static CryptoManager instance;

  return instance;
}

}  // namespace crypto
}  // namespace hub
