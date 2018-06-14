// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include <argon2.h>
#include <stdexcept>
#include <string>

#include "hub/auth/hmac_provider.h"
#include "hub/crypto/types.h"
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
  std::string key =
      "d42712f2b5a4321d34eed0b0503d618058041638d057f6b4f168574f6362df1c12694cd8"
      "234916349b6780e8abd00dc4f3ab7df0829468886c070b64ac38694f";
  HMACProvider provider(key);

  hub::crypto::Hash bundleHash(
      "9999999999999999999999999999999999999999999999999999999D9999999999999999"
      "999999999");
  hub::crypto::Address address(
      "999ZZS9LS99LPKLDGHTU999999PP9KH9K9JH999999PLR99IO999T999H9999999999HHL99"
      "9YU999ZBA");
  auth::SignBundleContext context(bundleHash, address);

  constexpr auto token =
      "$argon2id$v=19$m=131072,t=4,p=1$"
      "ZDQyNzEyZjJiNWE0MzIxZDM0ZWVkMGIwNTAzZDYxODA1ODA0MTYzOGQwNTdmNmI0ZjE2ODU3"
      "NGY2MzYyZGYxYzEyNjk0Y2Q4";

  ASSERT_TRUE(provider.validateToken(context, token));
}

TEST_F(HMACProviderTest, ShouldRejectOnIncorrectToken) {
  std::string key =
      "d42712f2b5a4321d34eed0b0503d618058041638d057f6b4f168574f6362df1c12694cd8"
      "234916349b6780e8abd00dc4f3ab7df0829468886c070b64ac38694f";
  HMACProvider provider(key);

  hub::crypto::Hash bundleHash(
      "9999999999999999999999999999999999999999999999999999999D9999999999999999"
      "999999999");
  hub::crypto::Address address(
      "999ZZS9LS99LPKLDGHTU999999PP9KH9K9JH999999PLR99IO999T999H9999999999HHL99"
      "9YU999ZBA");
  auth::SignBundleContext context(bundleHash, address);

  // changed last character
  constexpr auto token =
      "$argon2id$v=19$m=131072,t=4,p=1$"
      "ZDQyNzEyZjJiNWE0MzIxZDM0ZWVkMGIwNTAzZDYxODA1ODA0MTYzOGQwNTdmNmI0ZjE2ODU3"
      "NGY2MzYyZGYxYzEyNjk0Y2Q5";

  ASSERT_FALSE(provider.validateToken(context, token));
}

};  // namespace
