/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include <fstream>
#include <utility>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "hub/db/connection.h"
#include "hub/db/mariadb.h"
#include "hub/db/sqlite3.h"

#include "hub/db/db.h"

namespace {
thread_local static std::unique_ptr<hub::db::Connection> tl_connection;
}

namespace hub {
namespace db {

DEFINE_string(dbType, "mariadb", "Type of DB");
DEFINE_string(dbHost, "127.0.0.1", "Database server host");
DEFINE_uint32(dbPort, 3306, "Database server port");
DEFINE_string(db, "hub", "Database name");
DEFINE_string(dbUser, "user", "Database user");
DEFINE_string(dbPassword, "password", "Database user password");
DEFINE_bool(dbDebug, false, "Enable debug mode for database connection");

DBManager& DBManager::get() {
  static DBManager instance;

  return instance;
}

void DBManager::loadConnectionConfigFromArgs() {
  /*
  if (FLAGS_dbType == "sqlite3") {
    LOG(FATAL) << "sqlite3 is only supported for testing";
  }
  */
  LOG(INFO) << "Loading database connection config from database.";

  setConnectionConfig({FLAGS_dbType, FLAGS_dbHost, FLAGS_dbPort, FLAGS_db,
                       FLAGS_dbUser, FLAGS_dbPassword, FLAGS_dbDebug});
}

void DBManager::resetConnection() { tl_connection = nullptr; }

void DBManager::executeFile(const std::string& fname) {
  auto& conn = connection();

  std::ifstream fSchema(fname);
  std::string schema((std::istreambuf_iterator<char>(fSchema)),
                     std::istreambuf_iterator<char>());
  std::stringstream ss(schema);

  std::string line;
  std::string cmd;

  while (std::getline(ss, line, '\n')) {
    auto pos = line.find_first_not_of(' ');
    if (pos != std::string::npos && line[pos] == '-') {
      continue;
    }

    cmd += line;
    cmd += " ";

    if (line.find(';') != std::string::npos) {
      conn.execute(cmd);
      cmd = "";
    }
  }
}

void DBManager::loadSchema() {
  executeFile("schema/schema.sql");
  executeFile("schema/triggers." + _config.type + ".sql");

  LOG(INFO) << "Populated database with schema.";
}

Connection& DBManager::connection() {
  if (tl_connection) {
    return *tl_connection;
  }

  if (_config.type == "sqlite3") {
    tl_connection = std::make_unique<SQLite3Connection>(_config);
  } else if (_config.type == "mariadb") {
    tl_connection = std::make_unique<MariaDBConnection>(_config);
  } else {
    throw new std::runtime_error("Invalid DB schema: " + _config.type);
  }

  return *tl_connection;
}

}  // namespace db
}  // namespace hub
