// Copyright 2018 IOTA Foundation

#include "hub/db/sqlite3.h"

#include <memory>

#include <sqlpp11/sqlite3/connection.h>

#include "hub/db/db.h"

namespace hub {
namespace db {

namespace sqlite = sqlpp::sqlite3;

template <>
std::unique_ptr<sqlite::connection>
ConnectionImpl<sqlite::connection, sqlite::connection_config>::fromConfig(
    const Config& config) {
  sqlite::connection_config db;

  db.debug = config.debug;
  db.path_to_database = config.database;
  db.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

  auto conn = std::make_unique<sqlite::connection>(db);
  conn->set_default_isolation_level(sqlpp::isolation_level::serializable);

  return conn;
}

}  // namespace db
}  // namespace hub
