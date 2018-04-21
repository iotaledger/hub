#ifndef __HUB_CRYPTO_PROVIDER_H_
#define __HUB_CRYPTO_PROVIDER_H_

#include <boost/uuid/uuid.hpp>
#include <string>

namespace iota {
namespace crypto {

class CryptoProvider {
 public:
  virtual ~CryptoProvider() {}
  virtual std::string getAddressForUUID(const boost::uuids::uuid& uuid) const = 0;
};

}  // namespace crypto
}  // namespace iota
#endif /* __HUB_CRYPTO_PROVIDER_H_ */
