// Copyright 2018 IOTA Foundation

#include "hub/db/uuid.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <cstring>

namespace hub {
namespace db {

boost::uuids::uuid uuidFromData(const std::string& uuidStr) {
  return boost::lexical_cast<boost::uuids::uuid>(uuidStr);
}
std::string dataFromUuid(const boost::uuids::uuid& uuid) {
  return boost::lexical_cast<std::string>(uuid);
}
}  // namespace db
}  // namespace hub
