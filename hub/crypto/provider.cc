/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/crypto/provider.h"
#include "common/helpers/checksum.h"

namespace hub {
namespace crypto {

Checksum CryptoProvider::calcChecksum(std::string_view address) const {
  char* checksumPtr =
      iota_checksum(address.data(), address.size(), Checksum::length());
  auto result = Checksum(checksumPtr);
  std::free(checksumPtr);
  return result;
}

nonstd::optional<Address> CryptoProvider::verifyAndStripChecksum(
    const std::string& address) const {
  auto addressView = std::string_view(address).substr(0, Address::length());
  auto checksumView = std::string_view(address).substr(
      Address::length(), Address::length() + Checksum::length());
  if (calcChecksum(addressView).str_view() == checksumView) {
    return Address(address.substr(0, Address::length()));
  }
  return {};
}

}  // namespace crypto
}  // namespace hub
