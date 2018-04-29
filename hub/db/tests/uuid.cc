// Copyright 2018 IOTA Foundation

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "hub/db/uuid.h"

namespace {

class UUIDTest : public ::testing::Test {};

TEST_F(UUIDTest, UUIDFromDataIsConsistent) {
  auto originalUuid = boost::uuids::random_generator()();
  auto recreatedUuid =
      hub::db::uuidFromData(hub::db::dataFromUuid(originalUuid));
  EXPECT_EQ(originalUuid, recreatedUuid);
}
}  // namespace
