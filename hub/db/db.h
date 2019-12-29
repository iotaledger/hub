/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_DB_DB_H_
#define HUB_DB_DB_H_

#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include <gflags/gflags.h>

#include "hub/db/connection.h"
#include "hub/db/types.h"

namespace hub {
namespace db {
DECLARE_string(db);

struct Config {
  std::string type;

  std::string host;
  uint32_t port;
  std::string database;

  std::string user;
  std::string password;

  bool debug{false};
};

class Connection;

/***
    This class manages access to the database connection.
    It currently creates one connection per thread.
***/
class DBManager {
 private:
  DBManager() {}

  DBManager(DBManager const&) = delete;
  DBManager(DBManager&&) = delete;
  DBManager& operator=(DBManager const&) = delete;
  DBManager& operator=(DBManager&&) = delete;

 public:
  static DBManager& get();

  //! Loads schema
  void loadSchema();

  void resetConnection();

  void loadConnectionConfigFromArgs();
  void setConnectionConfig(Config config) { _config = std::move(config); }
  const Config& config() const { return _config; }

  hub::db::Connection& connection();

 protected:
  void executeFile(const std::string& fname);

 private:
  Config _config;
};

}  // namespace db
}  // namespace hub

#endif  // HUB_DB_DB_H_
