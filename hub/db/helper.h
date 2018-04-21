#ifndef __HUB_DB_HELPER_H_
#define __HUB_DB_HELPER_H_

#include <cstdint>
#include <memory>
#include <optional>
#include <sstream>
#include <string>

#include <sqlpp11/connection.h>
#include <sqlpp11/schema.h>
#include <sqlpp11/serialize.h>
#include <sqlpp11/sqlite3/connection.h>
#include <sqlpp11/transaction.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/select.h>

#include "db.h"
#include "schema/schema.h"

namespace iota {
namespace db {

inline std::optional<int64_t> userIdFromIdentifier(Connection& connection,
                                            const std::string& identifier) {
  using namespace sqlpp;

  db::sql::UserAccount acc;

  const auto result =
      connection(select(acc.id).from(acc).where(acc.identifier == identifier));

  if (result.empty()) {
    return {};
  } else {
    return result.front().id;
  }
}

}  // namespace db
}  // namespace iota

#endif /* __HUB_DB_HELPER_H_ */
