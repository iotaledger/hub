// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

#include "hub/crypto/types.h"
#include "hub/tests/runner.h"

using namespace hub;
using namespace hub::crypto;

namespace {

class TrytesArrayTest : public hub::Test {};

TEST_F(TrytesArrayTest, EnforceCorrectLength) {
  EXPECT_THROW(TryteArray<81>{""}, std::runtime_error);
}

TEST_F(TrytesArrayTest, EnforceValidCharacters) {
  EXPECT_THROW(TryteArray<12>{"WhatTheHell?"}, std::runtime_error);
}

TEST_F(TrytesArrayTest, AssertInternalRepresentationIsConsistent) {
  auto data =
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
  Hash testHash(data);
  ASSERT_EQ(testHash.toString(), data);
}

class UUIDTest : public hub::Test {};

TEST_F(UUIDTest, AssertInternalRepresentationIsConsistent) {
  auto uuidOrig = UUID::generate();
  auto stringRepresentation = uuidOrig.toString();
  auto uuidClone = UUID(stringRepresentation);
  ASSERT_EQ(uuidClone.toString(), stringRepresentation);
}

};  // namespace
