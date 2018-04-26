// Copyright 2018 IOTA Foundation

#ifndef HUB_DB_DB_H_
#define HUB_DB_DB_H_

#include <memory>
#include <sstream>
#include <string>

#include <gflags/gflags.h>
#include <sqlpp11/connection.h>
#include <sqlpp11/schema.h>
#include <sqlpp11/serialize.h>
#include <sqlpp11/sqlite3/connection.h>
#include <sqlpp11/transaction.h>
#include <sqlpp11/type_traits.h>

#include "hub/db/types.h"

namespace hub {
namespace db {
DECLARE_string(db);

using Connection = sqlpp::sqlite3::connection;

/***
    This class manages access to the database connection.
    It currently creates one connection per thread.
***/
class DBManager {
 private:
  DBManager() {}

 public:
  static DBManager& get();

  //! Loads schema
  void loadSchema(bool removeExisting);

  void resetConnection();

  void setConnection(std::unique_ptr<Connection> ptr);
  Connection& connection();
};

}  // namespace db
}  // namespace hub

#endif  // HUB_DB_DB_H_
