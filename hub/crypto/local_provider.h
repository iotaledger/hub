// Copyright 2018 IOTA Foundation

#ifndef HUB_CRYPTO_LOCAL_PROVIDER_H_
#define HUB_CRYPTO_LOCAL_PROVIDER_H_

#include <cstdint>
#include <string>

#include "hub/crypto/provider.h"

namespace hub {
namespace crypto {

class LocalProvider : public CryptoProvider {
 public:
  LocalProvider() = delete;
  explicit LocalProvider(std::string salt);

  Address getAddressForUUID(const UUID& uuid) const override;

 protected:
  std::string doGetSignatureForUUID(const UUID& uuid,
                                    const Hash& bundleHash) const override;

 private:
  const std::string _salt;

  static constexpr uint32_t _argon_t_cost{1};
  static constexpr uint32_t _argon_m_cost{1 << 16};  // 64mebibytes
  static constexpr uint32_t _argon_parallelism{1};
};

}  // namespace crypto
}  // namespace hub
#endif  // HUB_CRYPTO_LOCAL_PROVIDER_H_
