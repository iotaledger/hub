/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/auth/manager.h"

namespace hub {
namespace auth {

AuthManager& AuthManager::get() {
  static AuthManager instance;

  return instance;
}

}  // namespace auth
}  // namespace hub
