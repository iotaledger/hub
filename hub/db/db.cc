// Copyright 2018 IOTA Foundation

#include "hub/db/db.h"

#include <fstream>
#include <utility>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <sqlpp11/sqlite3/sqlite3.h>
#include <sqlpp11/sqlpp11.h>

namespace {
thread_local static std::unique_ptr<hub::db::Connection> tl_connection;
}

namespace hub {
namespace db {

DEFINE_string(db, "hub.db", "Path to sqlite3 database");
DEFINE_bool(dbInit, false, "Initialise db on startup");

DBManager& DBManager::get() {
  static DBManager instance;

  return instance;
}

void DBManager::resetConnection() { tl_connection = nullptr; }

void DBManager::setConnection(std::unique_ptr<Connection> ptr) {
  tl_connection = std::move(ptr);
}

void DBManager::loadSchema(bool removeExisting) {
  auto& conn = connection();

  std::ifstream fSchema("schema/schema.sql");
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

    if (line.find(';') != std::string::npos) {
      conn.execute(cmd);
      cmd = "";
    }
  }

  LOG(INFO) << "Populated database with schema.";
}

Connection& DBManager::connection() {
  if (tl_connection) {
    return *tl_connection;
  }

  sqlpp::sqlite3::connection_config config;
  config.path_to_database = FLAGS_db;
  config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

#ifdef DEBUG
  config.debug = true;
#endif

  LOG(INFO) << "Opened up database connection to " << FLAGS_db;

  tl_connection = std::make_unique<sqlpp::sqlite3::connection>(config);
  tl_connection->set_default_isolation_level(
      sqlpp::isolation_level::serializable);

  if (FLAGS_dbInit) {
    loadSchema(false);
    FLAGS_dbInit = false;
  }

  return *tl_connection;
}

}  // namespace db
}  // namespace hub
