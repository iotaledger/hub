// Copyright 2018 IOTA Foundation

#include "hub/crypto/local_provider.h"

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include <argon2.h>
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <iota/crypto/signing.hpp>
#include <iota/models/bundle.hpp>
#include <iota/types/trinary.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

#include "common/helpers/sign.h"
#include "common/kerl/converter.h"
#include "common/kerl/kerl.h"
#include "common/sign/v1/iss_kerl.h"
#include "common/trinary/trits.h"
#include "common/trinary/tryte.h"
#include "hub/crypto/types.h"

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

#ifndef FLAGS_maxConcurrentArgon2Hash
#define FLAGS_maxConcurrentArgon2Hash 4
#endif

boost::interprocess::interprocess_semaphore argon_semaphore(FLAGS_maxConcurrentArgon2Hash);

using TryteSeed = std::array<tryte_t, TRYTE_LEN + 1>;
using TryteSeedPtr =
    std::unique_ptr<TryteSeed, std::function<void(TryteSeed*)>>;

TryteSeedPtr seedFromUUID(const hub::crypto::UUID& uuid,
                          const std::string& _salt) {
  std::array<uint8_t, BYTE_LEN> byteSeed;
  std::array<trit_t, TRIT_LEN> seed;

  TryteSeedPtr tryteSeed(new TryteSeed{}, [](TryteSeed* seed) {
    seed->fill(0);
    delete seed;
  });

  argon_semaphore.wait();
  argon2i_hash_raw(_argon_t_cost, _argon_m_cost, _argon_parallelism,
                   uuid.str_view().data(), hub::crypto::UUID::UUID_SIZE,
                   _salt.c_str(), _salt.length(), byteSeed.data(), BYTE_LEN);
  argon_semaphore.post();

  bytes_to_trits(byteSeed.data(), seed.data());
  byteSeed.fill(0);

  tryteSeed->fill(0);
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

Address LocalProvider::getAddressForUUID(const hub::crypto::UUID& uuid) const {
  LOG(INFO) << "Generating address for: " << uuid.str().substr(0, 16);

  auto seed = seedFromUUID(uuid, _salt);
  auto add = iota_sign_address_gen((const char*)seed->data(), KEY_IDX, KEY_SEC);
  Address ret(add);
  std::free(add);
  return ret;
}

size_t LocalProvider::securityLevel() const { return KEY_SEC; }

std::string LocalProvider::doGetSignatureForUUID(const hub::crypto::UUID& uuid,
                                                 const Hash& bundleHash) const {
  LOG(INFO) << "Generating signature for: " << uuid.str().substr(0, 16)
            << ", bundle: " << bundleHash.str_view();

  auto seed = seedFromUUID(uuid, _salt);

  IOTA::Models::Bundle bundle;
  auto normalized = bundle.normalizedBundle(bundleHash.str());

  const size_t kKeyLength = ISS_KEY_LENGTH * KEY_SEC;
  Kerl kerl;
  init_kerl(&kerl);

  trit_t key[kKeyLength];
  std::array<trit_t, TRIT_LEN> tritSeed;
  trytes_to_trits(seed->data(), tritSeed.data(), TRYTE_LEN);
  iss_kerl_subseed(tritSeed.data(), tritSeed.data(), KEY_IDX, &kerl);
  iss_kerl_key(tritSeed.data(), key, kKeyLength, &kerl);
  tritSeed.fill(0);

  std::vector<int8_t> keyTrits(key, key + kKeyLength);
  std::ostringstream oss;

  for (size_t i = 0; i < KEY_SEC; i++) {
    std::vector<int8_t> bundleFrag(normalized.begin() + i * 27,
                                   normalized.begin() + (i + 1) * 27);
    std::vector<int8_t> keyFrag(keyTrits.begin() + i * ISS_KEY_LENGTH,
                                keyTrits.begin() + (i + 1) * ISS_KEY_LENGTH);

    auto sigFrag =
        IOTA::Crypto::Signing::signatureFragment(bundleFrag, keyFrag);

    oss << IOTA::Types::tritsToTrytes(sigFrag);
  }

  return oss.str();
}

}  // namespace crypto
}  // namespace hub
