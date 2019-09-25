/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_AUTH_HMAC_PROVIDER_H_
#define HUB_AUTH_HMAC_PROVIDER_H_

#include <cstdint>
#include <string>
#include <string_view>

#include "hub/auth/provider.h"
#include "hub/auth/sign_bundle_context.h"

namespace hub {
namespace auth {

/// DummyProvider class.
/// The dummy provider accepts any token.
class HMACProvider : public AuthProvider {
 public:
  /// 64bit based characters (43 *64bits = 258bits)
  constexpr static uint16_t KEY_SIZE = 43;
  /// 64bit based characters (43 *64bits = 258bits)
  constexpr static uint16_t HASH_SIZE = 43;
  /// Constructor
  explicit HMACProvider(const std::string& key);
  ~HMACProvider();

  bool validateToken(const AuthContext& context,
                     const std::string& token) noexcept override;

 private:
  std::string_view stripEncodedPrefix(const std::string_view& encoded);

  std::string _key;
};

}  // namespace auth
}  // namespace hub
#endif  // HUB_AUTH_HMAC_PROVIDER_H_
