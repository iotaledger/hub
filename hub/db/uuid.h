// Copyright 2018 IOTA Foundation

#ifndef HUB_DB_UUID_H_
#define HUB_DB_UUID_H_

#include <string>
#include <boost/uuid/uuid.hpp>

namespace hub {
namespace db {
boost::uuids::uuid uuidFromData(const std::string& uuidStr);
std::string dataFromUuid(const boost::uuids::uuid& uuid);
}  // namespace db
}  // namespace hub

#endif  // HUB_DB_UUID_H_
