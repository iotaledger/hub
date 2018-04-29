// Copyright 2018 IOTA Foundation

#include "hub/crypto/local_provider.h"

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>

#include <argon2.h>
#include <glog/logging.h>
#include <boost/uuid/uuid_io.hpp>

#include "common/helpers/sign.h"
#include "common/kerl/converter.h"
#include "common/trinary/trits.h"
#include "common/trinary/tryte.h"

// FIXME (th0br0) fix up entangled
extern "C" {
void trits_to_trytes(trit_t*, tryte_t*, size_t);
void trytes_to_trits(tryte_t*, trit_t*, size_t);
}

namespace {
static constexpr size_t BYTE_LEN = 48;
static constexpr size_t TRIT_LEN = 243;
static constexpr size_t TRYTE_LEN = 81;

static constexpr size_t KEY_IDX = 0;
static constexpr size_t KEY_SEC = 2;

static constexpr uint32_t _argon_t_cost = 1;
static constexpr uint32_t _argon_m_cost = 1 << 16;  // 64mebibytes
static constexpr uint32_t _argon_parallelism = 1;

using TryteSeed = std::array<tryte_t, TRYTE_LEN>;
using TryteSeedPtr =
    std::unique_ptr<TryteSeed, std::function<void(TryteSeed*)>>;

TryteSeedPtr seedFromUUID(const boost::uuids::uuid& uuid,
                          const std::string& _salt) {
  std::array<uint8_t, BYTE_LEN> byteSeed;
  std::array<trit_t, TRIT_LEN> seed;

  TryteSeedPtr tryteSeed(new TryteSeed{}, [](TryteSeed* seed) {
    seed->fill(0);
    delete seed;
  });

  argon2i_hash_raw(_argon_t_cost, _argon_m_cost, _argon_parallelism, &uuid.data,
                   uuid.size(), _salt.c_str(), _salt.length(), byteSeed.data(),
                   BYTE_LEN);

  bytes_to_trits(byteSeed.data(), seed.data());
  byteSeed.fill(0);

  trits_to_trytes(seed.data(), tryteSeed->data(), TRIT_LEN);
  seed.fill(0);

  return tryteSeed;
}
}  // namespace

namespace hub {
namespace crypto {

LocalProvider::LocalProvider(std::string salt) : _salt(std::move(salt)) {
  using std::string_literals::operator""s;

  if (_salt.length() < ARGON2_MIN_SALT_LENGTH) {
    throw std::runtime_error(
        "Invalid salt length: "s + std::to_string(_salt.length()) +
        " expected at least " + std::to_string(ARGON2_MIN_SALT_LENGTH));
  }
}

std::string LocalProvider::getAddressForUUID(
    const boost::uuids::uuid& uuid) const {
  LOG(INFO) << "Generating address for: " << uuid;
  return {iota_sign_address_gen((const char*)seedFromUUID(uuid, _salt)->data(),
                                KEY_IDX, KEY_SEC)};
}

std::string LocalProvider::doGetSignatureForUUID(
    const boost::uuids::uuid& uuid, const std::string& bundleHash) const {
  LOG(INFO) << "Generating signature for: " << uuid;

  return {
      iota_sign_signature_gen((const char*)seedFromUUID(uuid, _salt)->data(),
                              KEY_IDX, KEY_SEC, bundleHash.data())};
}

}  // namespace crypto
}  // namespace hub
