// Copyright 2018 IOTA Foundation

#ifndef HUB_DB_MYSQL_H_
#define HUB_DB_MYSQL_H_

#include <sqlpp11/mysql/connection.h>

#include "hub/db/connection.h"
#include "hub/db/connection_impl.h"

namespace hub {
namespace db {

class MySQLConnection : public ConnectionImpl<sqlpp::mysql::connection,
                                              sqlpp::mysql::connection_config> {
  using ConnectionImpl<sqlpp::mysql::connection,
                       sqlpp::mysql::connection_config>::ConnectionImpl;
};

}  // namespace db
}  // namespace hub

#endif  // HUB_DB_MYSQL_H_
