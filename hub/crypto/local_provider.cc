#include "local_provider.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>

#include <argon2.h>
#include "common/helpers/sign.h"
#include "common/kerl/converter.h"
#include "common/trinary/trits.h"
#include "common/trinary/tryte.h"

namespace {
static constexpr size_t BYTE_LEN = 48;
static constexpr size_t TRIT_LEN = 243;
static constexpr size_t TRYTE_LEN = 81;

static constexpr size_t KEY_IDX = 0;
static constexpr size_t KEY_SEC = 2;
}  // namespace

extern "C" {
void trits_to_trytes(trit_t*, tryte_t*, size_t);
void trytes_to_trits(tryte_t*, trit_t*, size_t);
}

namespace iota {
namespace crypto {
LocalProvider::LocalProvider(std::string salt) : _salt(std::move(salt)) {
  using namespace std::string_literals;

  if (_salt.length() < ARGON2_MIN_SALT_LENGTH) {
    throw std::runtime_error(
        "Invalid salt length: "s + std::to_string(_salt.length()) +
        " expected at least " + std::to_string(ARGON2_MIN_SALT_LENGTH));
  }
}

std::string LocalProvider::getAddressForUUID(
    const boost::uuids::uuid& uuid) const {
  std::array<uint8_t, BYTE_LEN> byteSeed;
  std::array<trit_t, TRIT_LEN> seed;
  std::array<tryte_t, TRYTE_LEN> tryteSeed;

  argon2i_hash_raw(_argon_t_cost, _argon_m_cost, _argon_parallelism, &uuid.data,
                   uuid.size(), _salt.c_str(), _salt.length(), byteSeed.data(), BYTE_LEN);
  
  bytes_to_trits(byteSeed.data(), seed.data());
  byteSeed.fill(0);

  trits_to_trytes(seed.data(), tryteSeed.data(), TRIT_LEN);
  seed.fill(0);

  std::string address(
      iota_sign_address_gen((const char*)tryteSeed.data(), KEY_IDX, KEY_SEC));
  tryteSeed.fill(0);

  return address;
}
}  // namespace crypto
}  // namespace iota
