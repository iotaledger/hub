/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_AUTH_SIGN_BUNDLE_CONTEXT_H_
#define HUB_AUTH_SIGN_BUNDLE_CONTEXT_H_

#include <string>

#include "common/crypto/types.h"
#include "hub/auth/provider.h"

namespace hub {
namespace auth {

class SignBundleContext : public AuthContext {
 public:
  SignBundleContext(const common::crypto::Hash& bundleHash,
                    const common::crypto::Address& address);
  const std::string& message() const noexcept override;

 private:
  std::string _message;
};

}  // namespace auth
}  // namespace hub
#endif  // HUB_AUTH_SIGN_BUNDLE_CONTEXT_H_
