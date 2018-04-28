// Copyright 2018 IOTA Foundation

#ifndef HUB_DB_HELPER_H_
#define HUB_DB_HELPER_H_

#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <sqlpp11/functions.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/schema.h>
#include <sqlpp11/select.h>
#include <sqlpp11/transaction.h>
#include <sqlpp11/update.h>
#include <boost/uuid/uuid.hpp>

#include "hub/db/db.h"
#include "hub/db/types.h"
#include "schema/schema.h"

namespace hub {
namespace db {

inline std::optional<int64_t> userIdFromIdentifier(
    Connection& connection, const std::string& identifier) {
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

inline std::vector<AddressWithID> unsweptUserAddresses(
    Connection& connection) {
  db::sql::UserAddress addr;
  db::sql::UserAddressBalance bal;

  std::vector<AddressWithID> addresses;

  auto result =
      connection(select(addr.id, addr.address)
                     .from(addr)
                     .where(!(exists(select(bal.id).from(bal).where(
                         bal.userAddress == addr.id && bal.reason == 1)))));

  for (const auto& row : result) {
    addresses.push_back({row.id, row.address});
  }

  return addresses;
}

inline std::vector<std::string> tailsForAddress(Connection& connection,
                                                uint64_t userId) {
  db::sql::UserAddressBalance bal;

  std::vector<std::string> tails;

  auto result = connection(
      select(bal.tailHash).from(bal).where(bal.userAddress == userId));

  for (const auto& row : result) {
    tails.push_back(std::move(row.tailHash));
  }

  return tails;
}

inline std::optional<uint64_t> availableBalanceForUser(
    Connection& connection, uint64_t userId) {
  db::sql::UserAccountBalance bal;

  const auto result = connection(select(sum(bal.amount).as(sqlpp::alias::a))
                                     .from(bal)
                                     .where(bal.userId == userId));

  if (result.empty()) {
    return {};
  }

  return result.front().a;
}

inline void createUserAddressBalanceEntry(Connection& connection,
                                          uint64_t addressId, int64_t amount,
                                          const UserAddressBalanceReason reason,
                                          std::optional<std::string> tailHash,
                                          std::optional<uint64_t> sweepId) {
  db::sql::UserAddressBalance bal;

  if (reason == UserAddressBalanceReason::DEPOSIT) {
    connection(
        insert_into(bal).set(bal.userAddress = addressId, bal.amount = amount,
                             bal.reason = static_cast<int>(reason),
                             bal.tailHash = std::move(tailHash.value())));
  } else {
    connection(insert_into(bal).set(
        bal.userAddress = addressId, bal.amount = amount,
        bal.reason = static_cast<int>(reason), bal.sweep = sweepId.value()));
  }
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
          .where(tbl.uuid == std::string(uuid.data, uuid.data + uuid.size()) &&
                 tbl.sweep.is_null()));
}

using AddressWithUUID = std::tuple<std::string, std::string>;
inline std::optional<AddressWithUUID> selectFirstUserAddress(
    Connection& connection) {
  db::sql::UserAddress addr;
  AddressWithUUID t;

  auto result = connection(select(addr.seedUuid, addr.address)
                               .from(addr)
                               .unconditionally()
                               .limit(1U));

  if (result.empty()) {
    return {};
  }

  const auto& row = *result.begin();
  return std::move(
      std::make_tuple(std::move(row.address), std::move(row.seedUuid)));
}

}  // namespace db
}  // namespace hub

#endif  // HUB_DB_HELPER_H_
