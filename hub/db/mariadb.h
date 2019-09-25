/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_DB_MARIADB_H_
#define HUB_DB_MARIADB_H_

#include <sqlpp11/mysql/connection.h>

#include "hub/db/connection.h"
#include "hub/db/connection_impl.h"

namespace hub {
namespace db {

class MariaDBConnection
    : public ConnectionImpl<sqlpp::mysql::connection,
                            sqlpp::mysql::connection_config> {
  using ConnectionImpl<sqlpp::mysql::connection,
                       sqlpp::mysql::connection_config>::ConnectionImpl;
};

}  // namespace db
}  // namespace hub

#endif  // HUB_DB_MARIADB_H_
