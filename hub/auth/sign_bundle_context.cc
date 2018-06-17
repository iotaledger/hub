/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/auth/sign_bundle_context.h"

namespace hub {
namespace auth {

SignBundleContext::SignBundleContext(const hub::crypto::Hash& bundleHash,
                                     const hub::crypto::Address& address) {
  _message = bundleHash.str() + address.str();
}
const std::string& SignBundleContext::message() const noexcept {
  return _message;
}
}  // namespace auth
}  // namespace hub
