// Copyright 2018 IOTA Foundation

#ifndef HUB_CRYPTO_PROVIDER_H_
#define HUB_CRYPTO_PROVIDER_H_

#include <string>

#include "hub/crypto/types.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"

namespace hub {
namespace crypto {

class CryptoProvider {
 public:
  virtual ~CryptoProvider() {}
  virtual Address getAddressForUUID(const UUID& uuid) const = 0;

  std::string getSignatureForUUID(hub::db::Connection& connection,
                                  const UUID& uuid,
                                  const Hash& bundleHash) const {
    connection.markUUIDAsSigned(uuid);

    return doGetSignatureForUUID(uuid, bundleHash);
  }

 protected:
  virtual std::string doGetSignatureForUUID(const UUID& uuid,
                                            const Hash& bundleHash) const = 0;
};

}  // namespace crypto
}  // namespace hub
#endif  // HUB_CRYPTO_PROVIDER_H_
