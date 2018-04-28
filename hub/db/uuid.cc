// Copyright 2018 IOTA Foundation

#include <cstring>
#include <boost/uuid/uuid_io.hpp>
#include "hub/db/uuid.h"

namespace hub {
namespace db {

boost::uuids::uuid uuidFromData(const std::string& uuidStr) {
  boost::uuids::uuid uuid;
  std::memcpy(&uuid.data, uuidStr.data(), uuid.size());
  return uuid;
}
std::string dataFromUuid(const boost::uuids::uuid& uuid) {
  return boost::uuids::to_string(uuid);
}
}  // namespace db
}  // namespace hub
