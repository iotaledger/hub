// Copyright 2018 IOTA Foundation

#include "hub/db/helper.h"

#include <sqlpp11/functions.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/schema.h>
#include <sqlpp11/select.h>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/transaction.h>
#include <sqlpp11/update.h>

#include <sqlpp11/mysql/connection.h>
#include <sqlpp11/sqlite3/connection.h>

namespace hub {
namespace db {

template <typename C>
std::optional<int64_t> helper<C>::userIdFromIdentifier(
    C& connection, const std::string& identifier) {
  db::sql::UserAccount acc;

  const auto result =
      connection(select(acc.id).from(acc).where(acc.identifier == identifier));

  if (result.empty()) {
    return {};
  } else {
    return result.front().id;
  }
}

template <typename C>
std::vector<AddressWithID> helper<C>::unsweptUserAddresses(C& connection) {
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

template <typename C>
std::vector<std::string> helper<C>::tailsForAddress(C& connection,
                                                    uint64_t userId) {
  db::sql::UserAddressBalance bal;

  std::vector<std::string> tails;

  auto result = connection(
      select(bal.tailHash).from(bal).where(bal.userAddress == userId));

  for (const auto& row : result) {
    tails.emplace_back(std::move(row.tailHash));
  }

  return tails;
}

template <typename C>
std::optional<uint64_t> helper<C>::availableBalanceForUser(C& connection,
                                                           uint64_t userId) {
  db::sql::UserAccount ua;

  const auto result =
      connection(select(ua.balance).from(ua).where(ua.id == userId));

  if (result.empty()) {
    return {};
  }

  return result.front().balance;
}

template <typename C>
void helper<C>::createUser(C& connection, const std::string& identifier) {
  db::sql::UserAccount userAccount;

  connection(insert_into(userAccount).set(userAccount.identifier = identifier));
}

template <typename C>
uint64_t helper<C>::createUserAddress(C& connection,
                                      const hub::crypto::Address& address,
                                      uint64_t userId,
                                      const hub::crypto::UUID& uuid) {
  db::sql::UserAddress userAddress;

  return connection(insert_into(userAddress)
                        .set(userAddress.address = address.str(),
                             userAddress.userId = userId,
                             userAddress.seedUuid = uuid.str()));
}

template <typename C>
void helper<C>::createUserAddressBalanceEntry(
    C& connection, uint64_t addressId, int64_t amount,
    const UserAddressBalanceReason reason, std::optional<std::string> tailHash,
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

template <typename C>
void helper<C>::createUserAccountBalanceEntry(
    C& connection, uint64_t userId, int64_t amount,
    const UserAccountBalanceReason reason, const std::optional<uint64_t> fkey) {
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

template <typename C>
uint64_t helper<C>::createWithdrawal(
    C& connection, const std::string& uuid, uint64_t userId, uint64_t amount,
    const hub::crypto::Address& payoutAddress) {
  db::sql::Withdrawal tbl;

  return connection(insert_into(tbl).set(
      tbl.uuid = uuid, tbl.userId = userId, tbl.amount = amount,
      tbl.payoutAddress = payoutAddress.str()));
}

template <typename C>
size_t helper<C>::cancelWithdrawal(C& connection, const std::string& uuid) {
  db::sql::Withdrawal tbl;
  auto now = ::sqlpp::chrono::floor<::std::chrono::milliseconds>(
      std::chrono::system_clock::now());

  return connection(update(tbl)
                        .set(tbl.cancelledAt = now)
                        .where(tbl.uuid == uuid && tbl.sweep.is_null()));
}

template <typename C>
std::optional<AddressWithUUID> helper<C>::selectFirstUserAddress(
    C& connection) {
  db::sql::UserAddress addr;
  auto result = connection(select(addr.seedUuid, addr.address)
                               .from(addr)
                               .unconditionally()
                               .limit(1U));

  if (result.empty()) {
    return {};
  }

  const auto& row = *result.begin();

  auto address = row.address;

  return std::make_tuple(std::move(row.address), row.seedUuid);
}

template <typename C>
void helper<C>::markUUIDAsSigned(C& connection, const hub::crypto::UUID& uuid) {
  db::sql::SignedUuids tbl;

  connection(insert_into(tbl).set(tbl.uuid = uuid.str()));
}

template <typename C>
std::vector<UserBalanceEvent> helper<C>::getUserAccountBalances(
    C& connection, uint64_t userId) {
  db::sql::UserAccountBalance bal;
  db::sql::UserAccount acc;
  auto result =
      connection(select(acc.identifier, bal.amount, bal.reason, bal.occuredAt)
                     .from(bal.join(acc).on(bal.userId == acc.id))
                     .where(bal.userId == userId));

  std::vector<UserBalanceEvent> balances;

  for (auto& row : result) {
    std::chrono::time_point<std::chrono::system_clock> ts =
        row.occuredAt.value();
    UserBalanceEvent event = {
        std::move(row.identifier), ts, row.amount,
        static_cast<UserAccountBalanceReason>((row.reason.value()))};
    balances.emplace_back(std::move(event));
  }
  return balances;
}

template <typename C>
std::vector<Sweep> helper<C>::getUnconfirmedSweeps(
    C& connection, const std::chrono::system_clock::time_point& olderThan) {
  db::sql::Sweep swp;
  db::sql::SweepTails tls;

  std::vector<Sweep> sweeps;

  auto result =
      connection(select(swp.id, swp.bundleHash, swp.trytes, swp.intoHubAddress)
                     .from(swp)
                     .where(swp.createdAt <= olderThan &&
                            !(exists(select(tls.hash).from(tls).where(
                                tls.sweep == swp.id && tls.confirmed == 1)))));

  constexpr size_t trytesPerTX = 2673;

  for (const auto& row : result) {
    // Need to chunk this up.
    std::vector<std::string> trytes;
    auto combinedTrytes = row.trytes.value();
    auto txCount = combinedTrytes.size() / trytesPerTX;

    for (size_t i = 0; i < txCount; i++) {
      trytes.push_back(
          combinedTrytes.substr(i * trytesPerTX, (i + 1) * trytesPerTX));
    }

    sweeps.push_back({static_cast<uint64_t>(row.id), std::move(row.bundleHash),
                      std::move(trytes),
                      static_cast<uint64_t>(row.intoHubAddress)});
  }

  return sweeps;
}

template <typename C>
void helper<C>::createTail(C& connection, uint64_t sweepId,
                           const std::string& hash) {
  db::sql::SweepTails tls;

  connection(insert_into(tls).set(tls.sweep = sweepId, tls.hash = hash));
}

template <typename C>
std::vector<std::string> helper<C>::getTailsForSweep(C& connection,
                                                     uint64_t sweepId) {
  db::sql::SweepTails tls;
  std::vector<std::string> tails;

  auto result = connection(select(tls.hash, tls.sweep, tls.createdAt)
                               .from(tls)
                               .where(tls.sweep == sweepId)
                               .order_by(tls.createdAt.desc()));

  for (const auto& row : result) {
    std::chrono::time_point<std::chrono::system_clock> ts =
        row.createdAt.value();
    tails.emplace_back(std::move(row.hash));
  }

  return tails;
}

template <typename C>
void helper<C>::markTailAsConfirmed(C& connection, const std::string& hash) {
  db::sql::SweepTails tbl;

  connection(update(tbl).set(tbl.confirmed = 1).where(tbl.hash == hash));
}

template <typename C>
std::vector<UserBalanceEvent> helper<C>::getAccountBalances(
    C& connection, std::chrono::system_clock::time_point newerThan) {
  db::sql::UserAccountBalance bal;
  db::sql::UserAccount acc;
  auto result =
      connection(select(acc.identifier, bal.amount, bal.reason, bal.occuredAt)
                     .from(bal.join(acc).on(bal.userId == acc.id))
                     .where(bal.occuredAt >= newerThan)
                     .order_by(bal.occuredAt.asc()));

  std::vector<UserBalanceEvent> balances;

  for (auto& row : result) {
    std::chrono::time_point<std::chrono::system_clock> ts =
        row.occuredAt.value();
    balances.emplace_back(UserBalanceEvent{
        std::move(row.identifier), ts, row.amount,
        static_cast<UserAccountBalanceReason>((row.reason.value()))});
  }
  return balances;
}

template <typename C>
void helper<C>::insertUserTransfers(
    C& connection, const std::vector<UserTransfer>& transfers) {
  db::sql::UserAccountBalance bal;

  if (transfers.empty()) {
    return;
  }
  auto multi_insert =
      insert_into(bal).columns(bal.userId, bal.amount, bal.reason);

  for (const auto& t : transfers) {
    auto reason = t.amount > 0 ? UserAccountBalanceReason::BUY
                               : UserAccountBalanceReason::SELL;
    multi_insert.values.add(bal.userId = static_cast<int>(t.userId),
                            bal.amount = t.amount,
                            bal.reason = static_cast<int>(reason));
  }
  connection(multi_insert);
}

template <typename C>
std::map<std::string, int64_t> helper<C>::userIdsFromIdentifiers(
    C& connection, const std::set<std::string>& identifiers) {
  db::sql::UserAccount acc;
  std::map<std::string, int64_t> identifierToId;
  auto result =
      connection(select(all_of(acc))
                     .from(acc)
                     .where(acc.identifier.in(sqlpp::value_list(identifiers))));

  for (auto& row : result) {
    identifierToId.insert(std::pair(row.identifier, row.id));
  }
  return identifierToId;
}

template <typename C>
std::map<uint64_t, int64_t> helper<C>::getTotalAmountForUsers(
    C& connection, const std::set<uint64_t>& ids) {
  db::sql::UserAccount ua;

  auto result = connection(select(ua.id, ua.balance)
                               .from(ua)
                               .where(ua.id.in(sqlpp::value_list(ids))));

  std::map<std::uint64_t, int64_t> identifierToTotal;
  for (auto& row : result) {
    identifierToTotal.insert(std::pair(row.id, row.balance));
  }
  return identifierToTotal;
}

template <typename C>
std::map<uint64_t, int64_t> helper<C>::getTotalAmountForAddresses(
    C& connection, const std::set<uint64_t>& ids) {
  db::sql::UserAddress ua;

  auto result = connection(select(ua.id, ua.balance)
                               .from(ua)
                               .where(ua.id.in(sqlpp::value_list(ids))));

  std::map<std::uint64_t, int64_t> addressIdToTotal;
  for (auto& row : result) {
    addressIdToTotal.insert(std::pair(row.id, row.balance));
  }
  return addressIdToTotal;
}

template <typename C>
WithdrawalInfo helper<C>::getWithdrawalInfoFromUUID(C& connection,
                                                    const std::string& uuid) {
  db::sql::Withdrawal tbl;

  auto result = connection(
      select(tbl.userId, tbl.amount).from(tbl).where(tbl.uuid == uuid));

  return {result.front().userId, result.front().amount};
}

template <typename C>
int64_t helper<C>::createHubAddress(C& connection,
                                    const hub::crypto::UUID& uuid,
                                    const std::string& address) {
  db::sql::HubAddress tbl;

  return connection(
      insert_into(tbl).set(tbl.seedUuid = uuid.str(), tbl.address = address));
}

template <typename C>
void helper<C>::createHubAddressBalanceEntry(
    C& connection, uint64_t hubAddress, int64_t amount,
    const HubAddressBalanceReason reason, uint64_t sweepId) {
  db::sql::HubAddressBalance bal;

  connection(insert_into(bal).set(
      bal.hubAddress = hubAddress, bal.amount = amount,
      bal.reason = static_cast<int>(reason), bal.sweep = sweepId));
}

template <typename C>
int64_t helper<C>::getHubAddressBalance(C& connection, uint64_t hubAddressId) {
  db::sql::HubAddress tbl;
  auto result =
      connection(select(tbl.balance).from(tbl).where(tbl.id == hubAddressId));
  return result.front().balance;
}

template <typename C>
int64_t helper<C>::getUserAddressBalance(C& connection,
                                         uint64_t userAddressId) {
  db::sql::UserAddress tbl;
  auto result =
      connection(select(tbl.balance).from(tbl).where(tbl.id == userAddressId));
  return result.front().balance;
}

template struct helper<sqlpp::mysql::connection>;
template struct helper<sqlpp::sqlite3::connection>;

}  // namespace db
}  // namespace hub
