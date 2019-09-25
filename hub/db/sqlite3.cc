/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include <memory>

#include <sqlpp11/sqlite3/connection.h>

#include "hub/db/db.h"

#include "hub/db/sqlite3.h"

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
  conn->execute("PRAGMA journal_mode = WAL");
  conn->execute("PRAGMA foreign_keys = ON");
  conn->execute("PRAGMA busy_timeout = 10000");

  return conn;
}

}  // namespace db
}  // namespace hub
