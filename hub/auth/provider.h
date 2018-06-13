/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_AUTH_PROVIDER_H_
#define HUB_AUTH_PROVIDER_H_

#include <string>

namespace hub {
namespace auth {

enum AuthContext { UNDEFINED = 0, SIGN_BUNDLE = 1 };

/// AuthProvider abstract class.
/// Provides facilities to authenticate certain requests.
class AuthProvider {
 public:
  /// Destructor
  virtual ~AuthProvider() {}

  /// Attempts to validate a token.
  /// param[in] the token context
  /// param[in] token the token
  /// returns true if token was valid
  virtual bool validateToken(const AuthContext context,
                             const std::string& token) noexcept = 0;
};

}  // namespace auth
}  // namespace hub
#endif  // HUB_AUTH_PROVIDER_H_
