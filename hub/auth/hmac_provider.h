/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_AUTH_HMAC_PROVIDER_H_
#define HUB_AUTH_HMAC_PROVIDER_H_

#include <cstdint>
#include <string>

#include "hub/auth/provider.h"
#include "hub/auth/sign_bundle_context.h"

namespace hub {
namespace auth {

/// DummyProvider class.
/// The dummy provider accepts any token.
class HMACProvider : public AuthProvider {
 public:
  constexpr static uint16_t HMAC_KEY_SIZE = 128;
  /// Constructor
  explicit HMACProvider(const std::string& key);
  ~HMACProvider();

  bool validateToken(const AuthContext& context,
                     const std::string& token) noexcept override;

 private:
  std::string _key;
};

}  // namespace auth
}  // namespace hub
#endif  // HUB_AUTH_HMAC_PROVIDER_H_
