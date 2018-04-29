// Copyright 2018 IOTA Foundation

#ifndef HUB_CRYPTO_PROVIDER_H_
#define HUB_CRYPTO_PROVIDER_H_

#include <string>

#include <boost/uuid/uuid.hpp>

#include "hub/db/db.h"
#include "hub/db/helper.h"

namespace hub {
namespace crypto {

class CryptoProvider {
 public:
  virtual ~CryptoProvider() {}
  virtual std::string getAddressForUUID(
      const boost::uuids::uuid& uuid) const = 0;

  std::string getSignatureForUUID(const boost::uuids::uuid& uuid,
                                  hub::db::Connection& connection,
                                  const std::string& bundleHash) const {
    db::markUUIDAsSigned(connection, uuid);

    return doGetSignatureForUUID(uuid, bundleHash);
  }

 protected:
  virtual std::string doGetSignatureForUUID(
      const boost::uuids::uuid& uuid, const std::string& bundleHash) const = 0;
};

}  // namespace crypto
}  // namespace hub
#endif  // HUB_CRYPTO_PROVIDER_H_
