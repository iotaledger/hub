/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */
#include <argon2.h>
#include <glog/logging.h>
#include <string>

#include "hub/auth/hmac_provider.h"
#include "hub/argon_flags.h"

namespace hub {
namespace auth {

HMACProvider::HMACProvider(const std::string& key) : _key(key) {
  if (_key.size() != HMAC_KEY_SIZE) {
    throw std::runtime_error(
        __FUNCTION__ +
        std::string("Provided HMAC key has wrong size (expected size: ") +
        std::to_string(HMAC_KEY_SIZE) +
        " observed size: " + std::to_string(_key.size()) + ")");
  }
}
HMACProvider::~HMACProvider() {
  _key.replace(0, _key.size(), _key.size(), '0');
}

bool HMACProvider::validateToken(const AuthContext& context,
                                 const std::string& token) noexcept {
  char encoded[HMAC_KEY_SIZE * 4];

  if (argon2id_hash_encoded(FLAGS_argon2TCost, FLAGS_argon2MCost,
                            FLAGS_argon2Parallelism, context.message().c_str(),
                            context.message().size(), _key.c_str(), _key.size(),
                            HMAC_KEY_SIZE, encoded,
                            HMAC_KEY_SIZE * 4) != ARGON2_OK) {
    LOG(ERROR) << "Failed in " << __FUNCTION__;
    return false;
  }

  encoded[HMAC_KEY_SIZE] = '\0';

  return token.compare(encoded) == 0;
}

}  // namespace auth
}  // namespace hub
