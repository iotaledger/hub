// Copyright 2018 IOTA Foundation

#include "hub/db/helper.h"

namespace hub {
namespace db {

std::optional<int64_t> userIdFromIdentifier(Connection& connection,
                                            const std::string& identifier) {
  db::sql::UserAccount acc;

  const auto result =
      connection(select(acc.id).from(acc).where(acc.identifier == identifier));

  if (result.empty()) {
    return {};
  } else {
    return result.front().id;
  }
}

std::vector<AddressWithID> unsweptUserAddresses(Connection& connection) {
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

std::vector<std::string> tailsForAddress(Connection& connection,
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

std::optional<uint64_t> availableBalanceForUser(Connection& connection,
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

void createUserAddressBalanceEntry(Connection& connection, uint64_t addressId,
                                   int64_t amount,
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

void createUserAccountBalanceEntry(Connection& connection, uint64_t userId,
                                   int64_t amount,
                                   const UserAccountBalanceReason reason,
                                   const std::optional<uint64_t> fkey) {
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

uint64_t createWithdrawal(Connection& connection,
                          const boost::uuids::uuid& uuid, uint64_t userId,
                          uint64_t amount, const std::string& payoutAddress) {
  db::sql::Withdrawal tbl;

  connection(insert_into(tbl).set(tbl.uuid = dataFromUuid(uuid),
                                  tbl.userId = userId, tbl.amount = amount,
                                  tbl.payoutAddress = payoutAddress));

  return connection.last_insert_id();
}

size_t cancelWithdrawal(Connection& connection,
                        const boost::uuids::uuid& uuid) {
  db::sql::Withdrawal tbl;
  auto now = ::sqlpp::chrono::floor<::std::chrono::milliseconds>(
      std::chrono::system_clock::now());

  return connection(
      update(tbl)
          .set(tbl.cancelledAt = now)
          .where(tbl.uuid == dataFromUuid(uuid) && tbl.sweep.is_null()));
}

using AddressWithUUID = std::tuple<std::string, boost::uuids::uuid>;
std::optional<AddressWithUUID> selectFirstUserAddress(Connection& connection) {
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

  return std::make_tuple(std::move(row.address), uuidFromData(row.seedUuid));
}

void markUUIDAsSigned(Connection& connection, const boost::uuids::uuid& uuid) {
  db::sql::SignedUuids tbl;

  connection(insert_into(tbl).set(tbl.uuid = dataFromUuid(uuid)));
}

std::vector<UserBalanceEvent> getUserAccountBalances(Connection& connection,
                                                     uint64_t userId) {
  db::sql::UserAccountBalance bal;
  auto result = connection(select(bal.amount, bal.reason, bal.occuredAt)
                               .from(bal)
                               .where(bal.userId == userId));

  std::vector<UserBalanceEvent> balances;

  for (auto& row : result) {
    std::chrono::time_point<std::chrono::system_clock> ts =
        row.occuredAt.value();
    UserBalanceEvent event = {
        ts, row.amount,
        static_cast<UserAccountBalanceReason>((row.reason.value()))};
    balances.emplace_back(std::move(event));
  }
  return balances;
}

std::vector<Sweep> getUnconfirmedSweeps(
    Connection& connection,
    const std::chrono::system_clock::time_point& olderThan) {
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

void createTail(Connection& connection, uint64_t sweepId,
                const std::string& hash) {
  db::sql::SweepTails tls;

  connection(insert_into(tls).set(tls.sweep = sweepId, tls.hash = hash));
}

std::vector<std::string> getTailsForSweep(Connection& connection,
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

void markTailAsConfirmed(Connection& connection, const std::string& hash) {
  db::sql::SweepTails tbl;

  connection(update(tbl).set(tbl.confirmed = 1).where(tbl.hash == hash));
}

}  // namespace db
}  // namespace hub
