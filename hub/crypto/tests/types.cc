// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

#include "common/types/types.h"
#include "hub/tests/runner.h"

using namespace hub;
using namespace hub::crypto;

namespace {

struct TestTag {};

using TestArr = TryteArray<12, TestTag>;

class TrytesArrayTest : public hub::Test {};

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
  Hash testHash(data);
  ASSERT_EQ(testHash.str(), data);
}

class UUIDTest : public hub::Test {};

TEST_F(UUIDTest, AssertInternalRepresentationIsConsistent) {
  UUID uuidOrig;
  std::string_view stringRepresentation = uuidOrig.str_view();

  auto uuidClone = UUID(stringRepresentation);

  ASSERT_EQ(uuidClone.str(), uuidOrig.str());
}

};  // namespace
