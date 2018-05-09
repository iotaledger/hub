// Copyright 2018 IOTA Foundation

#include <cstring>
#include <string>
#include <random>

#include "hub/crypto/random_generator.h"

namespace hub {
namespace crypto {

std::string generateBase64RandomString(uint32_t length) {
  std::string randomString;
  std::random_device rd;
  std::uniform_int_distribution<> index_dist(0, length - 1);

  for (auto i = 0; i < length; ++i) {
    randomString += base64_chars[index_dist(rd)];
  }

  return randomString;
}

}  // namespace crypto
}  // namespace hub
