/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_AUTH_MANAGER_H_
#define HUB_AUTH_MANAGER_H_

#include <memory>
#include <utility>

#include "hub/auth/provider.h"

namespace hub {
namespace auth {

/// AuthManager holds the single instance of a AuthProvider
class AuthManager {
 private:
  /// Constructor
  AuthManager() {}
  /// deleted
  AuthManager(AuthManager const&) = delete;
  /// deleted
  AuthManager(AuthManager&&) = delete;
  /// deleted
  AuthManager& operator=(AuthManager const&) = delete;
  /// deleted
  AuthManager& operator=(AuthManager&&) = delete;

 public:
  /// Get the singleton instance
  /// @return AuthManager - the singleton instance of AuthManager
  static AuthManager& get();

  /// Set the authentication provider
  /// @param[in] provider - an instance of AuthProvider
  void setProvider(std::unique_ptr<AuthProvider> provider) {
    _provider = std::move(provider);
  }

  /// Get the authentication provider
  /// @return AuthProvider - an instance of AuthProvider
  AuthProvider& provider() { return *_provider; }

 private:
  /// The authentication provider
  std::unique_ptr<AuthProvider> _provider;
};

}  // namespace auth
}  // namespace hub
#endif  // HUB_AUTH_MANAGER_H_
