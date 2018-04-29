// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "hub/crypto/local_provider.h"
#include "hub/db/db.h"
#include "hub/tests/runner.h"

using namespace hub;
using namespace hub::crypto;

namespace {

class LocalProviderTest : public hub::Test {};

TEST_F(LocalProviderTest, EnforceMinimumSeedLength) {
  EXPECT_THROW(LocalProvider{std::string("abcdefg")}, std::runtime_error);
}

TEST_F(LocalProviderTest, ShouldReturnValidAddress) {
  LocalProvider provider(std::string("abcdefgh"));
  boost::uuids::uuid uuid = boost::uuids::random_generator()();

  auto address = provider.getAddressForUUID(uuid);

  EXPECT_EQ(address.size(), 81);
}

TEST_F(LocalProviderTest, ConstantAddressForUUID) {
  LocalProvider provider(std::string("abcdefgh"));
  auto uuid = boost::uuids::random_generator()();

  auto address1 = provider.getAddressForUUID(uuid);
  auto address2 = provider.getAddressForUUID(uuid);

  EXPECT_EQ(address1, address2);
}

TEST_F(LocalProviderTest, DifferentUUIDsHaveDifferentAddresses) {
  LocalProvider provider(std::string("abcdefgh"));
  boost::uuids::uuid uuid1 = boost::uuids::random_generator()();
  boost::uuids::uuid uuid2 = boost::uuids::random_generator()();

  EXPECT_NE(uuid1, uuid2);

  auto address1 = provider.getAddressForUUID(uuid1);
  auto address2 = provider.getAddressForUUID(uuid2);

  EXPECT_NE(address1, address2);
}

TEST_F(LocalProviderTest, ShouldOnlySignOnce) {
  LocalProvider provider(std::string("abcdefgh"));
  boost::uuids::uuid uuid = boost::uuids::random_generator()();

  auto& connection = db::DBManager::get().connection();

  // First time should work.
  provider.getSignatureForUUID(uuid, connection,
                               "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                               "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

  // Second time should fail.
  ASSERT_THROW(
      provider.getSignatureForUUID(uuid, connection,
                                   "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
                                   "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"),
      std::exception);
}

};  // namespace
