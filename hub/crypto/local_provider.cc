/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/crypto/local_provider.h"

namespace hub {
namespace crypto {
nonstd::optional<std::string> LocalSigningProvider::getSignatureForUUID(
    const common::crypto::UUID& uuid,
    const common::crypto::Hash& bundleHash) const {
  auto& connection = db::DBManager::get().connection();

  connection.markUUIDAsSigned(uuid);

  return doGetSignatureForUUID(uuid, bundleHash);
}
}  // namespace crypto
}  // namespace hub
