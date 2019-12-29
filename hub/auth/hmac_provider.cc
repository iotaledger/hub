/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */
#include <argon2.h>
#include <glog/logging.h>
#include <string>

#include "common/flags.h"
#include "hub/auth/hmac_provider.h"

namespace hub {
namespace auth {

HMACProvider::HMACProvider(const std::string& key) : _key(key) {
  if (_key.size() > KEY_SIZE) {
    _key.resize(KEY_SIZE);
  } else if (_key.size() < KEY_SIZE) {
    throw std::runtime_error(
        __FUNCTION__ +
        std::string("Provided HMAC key has wrong size (expected size: ") +
        std::to_string(KEY_SIZE) +
        " observed size: " + std::to_string(_key.size()) + ")");
  }
}
HMACProvider::~HMACProvider() {
  _key.replace(0, _key.size(), _key.size(), '0');
}

bool HMACProvider::validateToken(const AuthContext& context,
                                 const std::string& token) noexcept {
  char encoded[HASH_SIZE * 6];

  if (argon2id_hash_encoded(
          common::flags::FLAGS_argon2TCost, common::flags::FLAGS_argon2MCost,
          common::flags::FLAGS_argon2Parallelism, context.message().c_str(),
          context.message().size(), _key.c_str(), _key.size(), HASH_SIZE * 2,
          encoded, HASH_SIZE * 6) != ARGON2_OK) {
    LOG(ERROR) << "Failed in " << __FUNCTION__;
    return false;
  }

  return token.compare(stripEncodedPrefix(encoded)) == 0;
}

std::string_view HMACProvider::stripEncodedPrefix(const std::string_view& e) {
  // strip prefix from encoded
  // Encoded example with prefix:
  // $argon2id$v=19$m=131072,t=4,p=1$c29tZXNhbHQ$xLJXnoh1+mb/bqTS72pL+pBbH1+sih7KFiZV1k4fdmM
  // After stripping: c29tZXNhbHQ$xLJXnoh1+mb/bqTS72pL+pBbH1+sih7KFiZV1k4fdmM

  static constexpr uint8_t NUM_DOLLAR_SIGNS_IN_PREFIX = 4;

  auto prefixSize = 0;
  uint8_t dollarSignCounter = 0;
  std::string_view curr(e);
  while (dollarSignCounter < NUM_DOLLAR_SIGNS_IN_PREFIX) {
    curr = e.substr(prefixSize, KEY_SIZE);
    auto pos = curr.find_first_of('$');
    if (pos == std::string::npos) {
      LOG(ERROR) << "Failed in " << __FUNCTION__;
      break;
    }
    curr = curr.substr(0, pos + 1);
    prefixSize += curr.size();
    dollarSignCounter++;
  }

  return e.substr(prefixSize, KEY_SIZE);
}

}  // namespace auth
}  // namespace hub
