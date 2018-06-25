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
  return Checksum(
      iota_checksum(address.data(), address.size(), Checksum::length()));
}

nonstd::optional<Address> CryptoProvider::verifyAndStripChecksum(
    const std::string& address) const {
  if (calcChecksum(std::string_view(address).substr(0, Address::length()))
          .str_view() ==
      std::string_view(address).substr(
          Address::length(), Address::length() + Checksum::length())) {
    return Address(address.substr(0, Address::length()));
  }
  return {};
}

}  // namespace crypto
}  // namespace hub
