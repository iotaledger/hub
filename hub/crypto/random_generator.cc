// Copyright 2018 IOTA Foundation

#include <cstring>
#include <string>

#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "hub/crypto/random_generator.h"

namespace hub {
namespace crypto {

std::string generateBase64RandomString(uint32_t length) {
  std::string randomString;
  boost::random::random_device rng;
  boost::random::uniform_int_distribution<> index_dist(0, length - 1);

  for (auto i = 0; i < length; ++i) {
    randomString += base64_chars[index_dist(rng)];
  }

  return randomString;
}

}  // namespace crypto
}  // namespace hub
