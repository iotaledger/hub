#ifndef __HUB_DB_HELPER_H_
#define __HUB_DB_HELPER_H_

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <sqlpp11/functions.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/update.h>
#include <sqlpp11/schema.h>
#include <sqlpp11/select.h>
#include <sqlpp11/transaction.h>
#include <boost/uuid/uuid.hpp>

#include "db.h"
#include "schema/schema.h"
#include "types.h"

namespace hub {
namespace db {

inline std::optional<int64_t> userIdFromIdentifier(
    Connection& connection, const std::string& identifier) {
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

using AddressWithID = std::tuple<uint64_t, std::string>;

inline std::vector<AddressWithID> unsweptUserAddresses(Connection& connection) {
  using namespace sqlpp;

  db::sql::UserAddress addr;
  db::sql::UserAddressBalance bal;

  std::vector<AddressWithID> addresses;

  auto result =
      connection(select(addr.id, addr.address)
                     .from(addr)
                     .where(not(exists(select(bal.id).from(bal).where(
                         bal.userAddress == addr.id and bal.reason == 1)))));

  for (const auto& row : result) {
    addresses.push_back({row.id, row.address});
  }

  return addresses;
}

inline std::optional<uint64_t> availableBalanceForUser(Connection& connection,
                                                       uint64_t userId) {
  db::sql::UserAccountBalance bal;

  const auto result = connection(select(sum(bal.amount).as(sqlpp::alias::a))
                                     .from(bal)
                                     .where(bal.userId == userId));

  if (result.empty()) {
    return {};
  }

  return result.front().a;
}

inline void createUserAccountBalanceEntry(
    Connection& connection, uint64_t userId, int64_t amount,
    const UserAccountBalanceReason reason,
    const std::optional<uint64_t> fkey = {}) {
  db::sql::UserAccountBalance bal;

  if (reason == UserAccountBalanceReason::SWEEP) {
    connection(insert_into(bal).set(bal.userId = userId, bal.amount = amount,
                                    bal.reason = static_cast<int>(reason),
                                    bal.sweep = fkey.value()));
  } else if (reason == UserAccountBalanceReason::WITHDRAWAL ||
             reason == UserAccountBalanceReason::WITHDRAWAL_CANCEL) {
    connection(insert_into(bal).set(bal.userId = userId, bal.amount = amount,
                                    bal.reason = static_cast<int>(reason),
                                    bal.withdrawal = fkey.value()));
  } else {
    connection(insert_into(bal).set(bal.userId = userId, bal.amount = amount,
                                    bal.reason = static_cast<int>(reason)));
  }
}

inline uint64_t createWithdrawal(Connection& connection,
                                 const boost::uuids::uuid& uuid,
                                 uint64_t userId, uint64_t amount,
                                 const std::string& payoutAddress) {
  db::sql::Withdrawal tbl;

  connection(insert_into(tbl).set(
      tbl.uuid = std::string(uuid.data, uuid.data + uuid.size()),
      tbl.userId = userId, tbl.amount = amount,
      tbl.payoutAddress = payoutAddress));

  return connection.last_insert_id();
}

inline size_t cancelWithdrawal(Connection& connection,
                                        const boost::uuids::uuid& uuid) {
  db::sql::Withdrawal tbl;
  auto now = ::sqlpp::chrono::floor<::std::chrono::milliseconds>(
      std::chrono::system_clock::now());

  return connection(
      update(tbl)
          .set(tbl.cancelledAt = now)
          .where(tbl.uuid == std::string(uuid.data, uuid.data + uuid.size()) and
                 tbl.sweep.is_null()));
}

}  // namespace db
}  // namespace hub

#endif /* __HUB_DB_HELPER_H_ */
