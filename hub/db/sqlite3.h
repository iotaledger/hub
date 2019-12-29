/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_DB_SQLITE3_H_
#define HUB_DB_SQLITE3_H_

#include <sqlpp11/sqlite3/connection.h>

#include "hub/db/connection.h"
#include "hub/db/connection_impl.h"

namespace hub {
namespace db {

class SQLite3Connection
    : public ConnectionImpl<sqlpp::sqlite3::connection,
                            sqlpp::sqlite3::connection_config> {
  using ConnectionImpl<sqlpp::sqlite3::connection,
                       sqlpp::sqlite3::connection_config>::ConnectionImpl;
};

}  // namespace db
}  // namespace hub

#endif  // HUB_DB_SQLITE3_H_
