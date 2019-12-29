/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/crypto/provider_base.h"
#include "common/helpers/checksum.h"

namespace common {
namespace crypto {

common::crypto::Checksum CryptoProviderBase::calcChecksum(
    std::string_view address) const {
  char* checksumPtr = iota_checksum(address.data(), address.size(),
                                    common::crypto::Checksum::length());
  auto result = common::crypto::Checksum(checksumPtr);
  std::free(checksumPtr);
  return result;
}

nonstd::optional<common::crypto::Address>
CryptoProviderBase::verifyAndStripChecksum(const std::string& address) const {
  auto addressView =
      std::string_view(address).substr(0, common::crypto::Address::length());
  auto checksumView = std::string_view(address).substr(
      common::crypto::Address::length(),
      common::crypto::Address::length() + common::crypto::Checksum::length());
  if (calcChecksum(addressView).str_view() == checksumView) {
    return common::crypto::Address(
        address.substr(0, common::crypto::Address::length()));
  }
  return {};
}

}  // namespace crypto
}  // namespace common
