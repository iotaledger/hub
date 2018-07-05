// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

#include "common/crypto/types.h"

namespace {

struct TestTag {};

using TestArr = common::crypto::TryteArray<12, TestTag>;

class TrytesArrayTest : public ::testing::Test {};

TEST_F(TrytesArrayTest, EnforceCorrectLength) {
  EXPECT_THROW(TestArr{""}, std::runtime_error);
}

TEST_F(TrytesArrayTest, EnforceValidCharacters) {
  EXPECT_THROW(TestArr{"WhatTheHell?"}, std::runtime_error);
}

TEST_F(TrytesArrayTest, AssertInternalRepresentationIsConsistent) {
  auto data =
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
  common::crypto::Hash testHash(data);
  ASSERT_EQ(testHash.str(), data);
}

class UUIDTest : public ::testing::Test {};

TEST_F(UUIDTest, AssertInternalRepresentationIsConsistent) {
  common::crypto::UUID uuidOrig;
  std::string_view stringRepresentation = uuidOrig.str_view();

  auto uuidClone = common::crypto::UUID(stringRepresentation);

  ASSERT_EQ(uuidClone.str(), uuidOrig.str());
}

};  // namespace
