// Copyright 2018 IOTA Foundation

#ifndef HUB_CRYPTO_PROVIDER_H_
#define HUB_CRYPTO_PROVIDER_H_

#include <string>

#include <boost/uuid/uuid.hpp>

namespace hub {
namespace crypto {

class CryptoProvider {
 public:
  virtual ~CryptoProvider() {}
  virtual std::string getAddressForUUID(
      const boost::uuids::uuid& uuid) const = 0;
};

}  // namespace crypto
}  // namespace hub
#endif  // HUB_CRYPTO_PROVIDER_H_
