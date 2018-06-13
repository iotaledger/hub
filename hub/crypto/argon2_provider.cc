/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/crypto/argon2_provider.h"

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include <argon2.h>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <iota/crypto/signing.hpp>
#include <iota/models/bundle.hpp>
#include <iota/types/trinary.hpp>

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

DEFINE_uint32(maxConcurrentArgon2Hash, 4,
              "Max number of concurrent Argon2 Hash processes");
DEFINE_uint32(argon2TCost, 4, "Time cost of Argon2");
DEFINE_uint32(argon2MCost, 1 << 17, "Memory cost of Argon2 in bytes");
DEFINE_uint32(argon2Parallelism, 1,
              "Number of threads to use in parallel for Argon2");
DEFINE_uint32(argon2Mode, 2, "Argon2 mode to use: 1=argon2i;2,else=argon2id");

using TryteSeed = std::array<tryte_t, TRYTE_LEN + 1>;
using TryteSeedPtr =
    std::unique_ptr<TryteSeed, std::function<void(TryteSeed*)>>;

TryteSeedPtr seedFromUUID(const hub::crypto::UUID& uuid,
                          const std::string& _salt) {
  static boost::interprocess::interprocess_semaphore argon_semaphore(
      FLAGS_maxConcurrentArgon2Hash);

  std::array<uint8_t, BYTE_LEN> byteSeed;
  std::array<trit_t, TRIT_LEN> seed;

  TryteSeedPtr tryteSeed(new TryteSeed{}, [](TryteSeed* seed) {
    seed->fill(0);
    delete seed;
  });

  argon_semaphore.wait();

  switch (FLAGS_argon2Mode) {
    case 1:
      argon2i_hash_raw(FLAGS_argon2TCost, FLAGS_argon2MCost,
                       FLAGS_argon2Parallelism, uuid.str_view().data(),
                       hub::crypto::UUID::UUID_SIZE, _salt.c_str(),
                       _salt.length(), byteSeed.data(), BYTE_LEN);
      break;
    default:
    case 2:
      argon2id_hash_raw(FLAGS_argon2TCost, FLAGS_argon2MCost,
                        FLAGS_argon2Parallelism, uuid.str_view().data(),
                        hub::crypto::UUID::UUID_SIZE, _salt.c_str(),
                        _salt.length(), byteSeed.data(), BYTE_LEN);
      break;
  }
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

Argon2Provider::Argon2Provider(std::string salt) : _salt(std::move(salt)) {
  using std::string_literals::operator""s;

  LOG(INFO) << "Initialising Argon2 provider in mode: " << FLAGS_argon2Mode;

  if (_salt.length() < ARGON2_MIN_SALT_LENGTH) {
    throw std::runtime_error(
        "Invalid salt length: "s + std::to_string(_salt.length()) +
        " expected at least " + std::to_string(ARGON2_MIN_SALT_LENGTH));
  }
}

Address Argon2Provider::getAddressForUUID(const hub::crypto::UUID& uuid) const {
  LOG(INFO) << "Generating address for: " << uuid.str().substr(0, 16);

  auto seed = seedFromUUID(uuid, _salt);
  auto add = iota_sign_address_gen((const char*)seed->data(), KEY_IDX, KEY_SEC);
  Address ret(add);
  std::free(add);
  return ret;
}

size_t Argon2Provider::securityLevel() const { return KEY_SEC; }

std::string Argon2Provider::doGetSignatureForUUID(
    const hub::crypto::UUID& uuid, const Hash& bundleHash) const {
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
