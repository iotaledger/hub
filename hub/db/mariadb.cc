/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include <memory>
#include <utility>

#include <sqlpp11/mysql/connection.h>

#include "hub/db/db.h"
#include "hub/db/mariadb.h"

namespace hub {
namespace db {

namespace mysql = sqlpp::mysql;

template <>
std::unique_ptr<mysql::connection>
ConnectionImpl<mysql::connection, mysql::connection_config>::fromConfig(
    const Config& config) {
  auto db = std::make_shared<mysql::connection_config>();
  db->debug = config.debug;

  db->user = config.user;
  db->password = config.password;
  db->host = config.host;
  db->port = config.port;
  db->database = config.database;

  auto conn = std::make_unique<mysql::connection>(std::move(db));
  conn->execute("SET SESSION TRANSACTION ISOLATION LEVEL SERIALIZABLE");

  return conn;
}

}  // namespace db
}  // namespace hub
