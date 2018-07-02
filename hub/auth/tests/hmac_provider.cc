// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include <argon2.h>
#include <stdexcept>
#include <string>

#include "hub/auth/hmac_provider.h"
#include "common/types/types.h"
#include "hub/db/db.h"
#include "hub/tests/runner.h"

using namespace hub;
using namespace hub::auth;

namespace {

class HMACProviderTest : public hub::Test {};

TEST_F(HMACProviderTest, EnforceKeyLength) {
  EXPECT_THROW(HMACProvider{std::string("abcdefghijklmnop")},
               std::runtime_error);
}

TEST_F(HMACProviderTest, ShouldValidateOnCorrectToken) {
  std::string key = "El6vxEO4rR009/U/u70SgPa6C7GVZQzXZOUQrkMnXFI";
  HMACProvider provider(key);

  common::crypto::Hash bundleHash(
      "9999999999999999999999999999999999999999999999999999999D9999999999999999"
      "999999999");
  common::crypto::Address address(
      "999ZZS9LS99LPKLDGHTU999999PP9KH9K9JH999999PLR99IO999T999H9999999999HHL99"
      "9YU999ZBA");
  auth::SignBundleContext context(bundleHash, address);

  constexpr auto token = "RWw2dnhFTzRyUjAwOS9VL3U3MFNnUGE2QzdHVlpRelh";

  ASSERT_TRUE(provider.validateToken(context, token));
}

TEST_F(HMACProviderTest, ShouldRejectOnIncorrectToken) {
  std::string key = "El6vxEO4rR009/U/u70SgPa6C7GVZQzXZOUQrkMnXFI";
  HMACProvider provider(key);

  common::crypto::Hash bundleHash(
      "9999999999999999999999999999999999999999999999999999999D9999999999999999"
      "999999999");
  common::crypto::Address address(
      "999ZZS9LS99LPKLDGHTU999999PP9KH9K9JH999999PLR99IO999T999H9999999999HHL99"
      "9YU999ZBA");
  auth::SignBundleContext context(bundleHash, address);

  // changed last character
  constexpr auto token = "RWw2dnhFTzRyUjAwOS9VL3U3MFNnUGE2QzdHVlpRelz";

  ASSERT_FALSE(provider.validateToken(context, token));
}

};  // namespace
