/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/db/helper.h"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
nonstd::optional<int64_t> helper<C>::userIdFromIdentifier(
    C& connection, const std::string& identifier) {
  db::sql::UserAccount acc;

  const auto result =
      connection(select(acc.id).from(acc).where(acc.identifier == identifier));

  if (result.empty()) {
    return {};
  } else {
    return result.front().id.value();
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
std::unordered_multimap<uint64_t, std::string> helper<C>::tailsForUserAddresses(
    C& connection, const std::vector<uint64_t>& userIds) {
  db::sql::UserAddressBalance bal;

  std::unordered_multimap<uint64_t, std::string> userIdsToTails;

  auto result =
      connection(select(bal.tailHash, bal.userAddress)
                     .from(bal)
                     .where(bal.userAddress.in(sqlpp::value_list(userIds))));

  for (const auto& row : result) {
    userIdsToTails.emplace(std::pair(row.userAddress, std::move(row.tailHash)));
  }

  return userIdsToTails;
}

template <typename C>
uint64_t helper<C>::availableBalanceForUser(C& connection, uint64_t userId) {
  db::sql::UserAccount ua;

  const auto result =
      connection(select(ua.balance).from(ua).where(ua.id == userId));

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
    const UserAddressBalanceReason reason,
    nonstd::optional<std::string> tailHash,
    nonstd::optional<uint64_t> sweepId) {
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
    const UserAccountBalanceReason reason,
    const nonstd::optional<uint64_t> fkey) {
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
    const hub::crypto::Tag& tag, const hub::crypto::Address& payoutAddress) {
  db::sql::Withdrawal tbl;

  return connection(insert_into(tbl).set(
      tbl.uuid = uuid, tbl.userId = userId, tbl.amount = amount,
      tbl.payoutAddress = payoutAddress.str(), tbl.tag = tag.str()));
}

template <typename C>
size_t helper<C>::cancelWithdrawal(C& connection, const std::string& uuid) {
  db::sql::Withdrawal tbl;
  auto now = ::sqlpp::chrono::floor<::std::chrono::milliseconds>(
      std::chrono::system_clock::now());

  return connection(update(tbl)
                        .set(tbl.cancelledAt = now)
                        .where(tbl.uuid == uuid && tbl.sweep.is_null() &&
                               tbl.cancelledAt.is_null()));
}

template <typename C>
nonstd::optional<AddressWithUUID> helper<C>::selectFirstUserAddress(
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

  return std::make_tuple(std::move(row.address.value()), row.seedUuid.value());
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
      trytes.push_back(combinedTrytes.substr(i * trytesPerTX, trytesPerTX));
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
std::vector<SweepEvent> helper<C>::getSweeps(
    C& connection, std::chrono::system_clock::time_point newerThan) {
  db::sql::Sweep swp;
  db::sql::Withdrawal withdrawal;
  auto result = connection(
      select(swp.id, swp.bundleHash, swp.createdAt, withdrawal.uuid)
          .from(swp.left_outer_join(withdrawal).on(swp.id == withdrawal.sweep))
          .where(swp.createdAt >= newerThan)
          .order_by(swp.id.asc()));

  std::unordered_map<std::string, std::vector<std::string>> hashToUUIDs;

  int64_t id = -1;
  std::vector<SweepEvent> events;
  SweepEvent* currEvent;

  for (auto& row : result) {
    if (row.id != id) {
      id = row.id;
      std::vector<std::string> uuids;
      std::chrono::time_point<std::chrono::system_clock> ts =
          row.createdAt.value();
      currEvent = &events.emplace_back(
          SweepEvent{std::move(row.bundleHash), ts, std::move(uuids)});
    }

    if (!std::string{row.uuid}.empty()) {
      auto& uuids = currEvent->withdrawalUUIDs;
      uuids.emplace_back(std::string{std::move(row.uuid)});
    }
  }

  return events;
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
      select(tbl.id, tbl.userId, tbl.amount).from(tbl).where(tbl.uuid == uuid));

  return {static_cast<uint64_t>(result.front().id),
          static_cast<uint64_t>(result.front().userId), result.front().amount};
}

template <typename C>
int64_t helper<C>::createHubAddress(C& connection,
                                    const hub::crypto::UUID& uuid,
                                    const hub::crypto::Address& address) {
  db::sql::HubAddress tbl;

  return connection(insert_into(tbl).set(tbl.seedUuid = uuid.str(),
                                         tbl.address = address.str()));
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
uint64_t helper<C>::getHubAddressBalance(C& connection, uint64_t hubAddressId) {
  db::sql::HubAddress tbl;
  auto result =
      connection(select(tbl.balance).from(tbl).where(tbl.id == hubAddressId));
  return static_cast<uint64_t>(result.front().balance);
}

template <typename C>
uint64_t helper<C>::getUserAddressBalance(C& connection,
                                          uint64_t userAddressId) {
  db::sql::UserAddress tbl;
  auto result =
      connection(select(tbl.balance).from(tbl).where(tbl.id == userAddressId));
  return static_cast<uint64_t>(result.front().balance);
}

template <typename C>
int64_t helper<C>::createSweep(C& connection,
                               const hub::crypto::Hash& bundleHash,
                               const std::string& bundleTrytes,
                               uint64_t intoHubAddress) {
  db::sql::Sweep tbl;

  return connection(insert_into(tbl).set(tbl.bundleHash = bundleHash.str(),
                                         tbl.trytes = bundleTrytes,
                                         tbl.intoHubAddress = intoHubAddress));
}

template <typename C>
std::vector<TransferOutput> helper<C>::getWithdrawalsForSweep(
    C& connection, size_t max,
    const std::chrono::system_clock::time_point& olderThan) {
  db::sql::Withdrawal tbl;

  std::vector<TransferOutput> outputs;

  auto result =
      connection(select(tbl.id, tbl.amount, tbl.tag, tbl.payoutAddress)
                     .from(tbl)
                     .where(tbl.requestedAt <= olderThan && tbl.sweep.is_null())
                     .order_by(tbl.requestedAt.asc())
                     .limit(max));

  for (const auto& row : result) {
    nonstd::optional<hub::crypto::Tag> maybeTag;

    if (!row.tag.is_null()) {
      maybeTag = hub::crypto::Tag(row.tag.value());
    }

    outputs.emplace_back(TransferOutput{
        row.id, static_cast<uint64_t>(row.amount), std::move(maybeTag),
        hub::crypto::Address(row.payoutAddress.value())});
  }

  return outputs;
}

template <typename C>
size_t helper<C>::setWithdrawalSweep(C& connection, uint64_t id,
                                     uint64_t sweepId) {
  db::sql::Withdrawal tbl;

  return connection(update(tbl).set(tbl.sweep = sweepId).where(tbl.id == id));
}

template <typename C>
std::vector<TransferInput> helper<C>::getDepositsForSweep(
    C& connection, size_t max,
    const std::chrono::system_clock::time_point& olderThan) {
  db::sql::UserAddress add;
  db::sql::UserAddressBalance bal;

  auto result = connection(
      select(add.id, add.address, add.userId, add.seedUuid, add.balance)
          .from(add)
          .where(add.balance > 0 &&
                 !exists(select(bal.id).from(bal).where(
                     bal.userAddress == add.id &&
                     bal.reason ==
                         static_cast<int>(UserAddressBalanceReason::SWEEP))))
          .limit(max));

  std::vector<TransferInput> deposits;

  for (const auto& row : result) {
    TransferInput ti = {row.id, row.userId,
                        hub::crypto::Address(row.address.value()),
                        hub::crypto::UUID(row.seedUuid.value()),
                        static_cast<uint64_t>(row.balance)};
    deposits.push_back(std::move(ti));
  }

  return deposits;
}

template <typename C>
std::vector<TransferInput> helper<C>::getHubInputsForSweep(
    C& connection, uint64_t requiredAmount,
    const std::chrono::system_clock::time_point& olderThan) {
  db::sql::HubAddress add;
  db::sql::HubAddressBalance bal;
  db::sql::Sweep swp;

  // 1. Get all available unused Hub addresses
  auto availableAddressesResult = connection(
      select(add.id, add.address, add.balance, add.seedUuid)
          .from(add)
          .where(add.createdAt < olderThan && add.isColdStorage == 0 &&
                 add.balance > 0 &&
                 !exists(select(bal.id).from(bal).where(
                     bal.hubAddress == add.id &&
                     bal.reason == static_cast<int>(
                                       HubAddressBalanceReason::OUTBOUND)))));

  std::vector<TransferInput> availableInputs;
  std::vector<int64_t> addressIds;
  for (const auto& row : availableAddressesResult) {
    auto id = row.id;

    TransferInput input = {id, 0, hub::crypto::Address(row.address.value()),
                           hub::crypto::UUID(row.seedUuid.value()),
                           static_cast<uint64_t>(row.balance)};

    addressIds.push_back(id);
    availableInputs.emplace_back(std::move(input));
  }

  // 2. Only select those with all INBOUND sweeps confirmed
  auto confirmedAddresses = connection(
      select(swp.intoHubAddress)
          .where(swp.intoHubAddress.in(sqlpp::value_list(addressIds)))
          .from(swp)
          .group_by(swp.intoHubAddress)
          .having(sum(swp.confirmed) == count(swp.id)));

  std::unordered_set<int64_t> confirmedAddressIds;

  for (const auto& row : confirmedAddresses) {
    confirmedAddressIds.insert(row.intoHubAddress);
  }

  availableInputs.erase(
      std::remove_if(availableInputs.begin(), availableInputs.end(),
                     [&confirmedAddressIds](const auto& input) {
                       return confirmedAddressIds.find(input.addressId) ==
                              confirmedAddressIds.end();
                     }),
      availableInputs.end());

  // Sort lowest to highest
  std::sort(availableInputs.begin(), availableInputs.end(),
            [](const auto& a, const auto& b) { return a.amount < b.amount; });

  std::vector<TransferInput> selectedInputs;
  auto it = availableInputs.begin();

  uint64_t total = 0;
  while (it != std::end(availableInputs) && total < requiredAmount) {
    total += it->amount;
    selectedInputs.emplace_back(std::move(*it));
    it++;
  }

  return selectedInputs;
}

template <typename C>
bool helper<C>::isSweepConfirmed(C& connection, uint64_t sweepId) {
  db::sql::Sweep swp;

  auto result =
      connection(select(swp.confirmed).from(swp).where(swp.id == sweepId));

  return result.front().confirmed;
}

template <typename C>
nonstd::optional<AddressInfo> helper<C>::getAddressInfo(
    C& connection, const hub::crypto::Address& address) {
  db::sql::UserAddress add;
  db::sql::UserAccount acc;

  auto result = connection(select(acc.identifier)
                               .from(add.join(acc).on(add.userId == acc.id))
                               .where(add.address == address.str()));

  if (result.empty()) {
    return {};
  } else {
    return {AddressInfo{std::move(result.front().identifier.value())}};
  }
}

template struct helper<sqlpp::mysql::connection>;
template struct helper<sqlpp::sqlite3::connection>;

}  // namespace db
}  // namespace hub
