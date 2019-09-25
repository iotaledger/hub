/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_AUTH_DUMMY_PROVIDER_H_
#define HUB_AUTH_DUMMY_PROVIDER_H_

#include <cstdint>
#include <string>

#include "hub/auth/provider.h"

namespace hub {
namespace auth {

/// DummyProvider class.
/// The dummy provider accepts any token.
class DummyProvider : public AuthProvider {
 public:
  /// Constructor
  DummyProvider() {}

  bool validateToken(const AuthContext& context,
                     const std::string& token) noexcept override {
    return true;
  }
};

}  // namespace auth
}  // namespace hub
#endif  // HUB_AUTH_DUMMY_PROVIDER_H_
