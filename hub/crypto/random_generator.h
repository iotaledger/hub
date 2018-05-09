// Copyright 2018 IOTA Foundation

#ifndef HUB_CRYPTO_RANDOM_GENERATOR_H_
#define HUB_CRYPTO_RANDOM_GENERATOR_H_

#include <string>

#include "hub/db/db.h"
#include "hub/db/helper.h"

namespace hub {
namespace crypto {

static constexpr auto base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static constexpr uint32_t base64_chars_for_384_bits = 64;

std::string generateBase64RandomString(uint32_t length);

}  // namespace crypto
}  // namespace hub
#endif  // HUB_CRYPTO_RANDOM_GENERATOR_H_
