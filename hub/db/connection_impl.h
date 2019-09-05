/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_DB_CONNECTION_IMPL_H_
#define HUB_DB_CONNECTION_IMPL_H_

#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <nonstd/optional.hpp>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <sqlpp11/transaction.h>

#include "common/crypto/types.h"
#include "hub/db/connection.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/db/types.h"

namespace hub {
namespace db {

template <typename Conn>
class TransactionImpl : public Transaction {
 public:
  explicit TransactionImpl(Conn& connection) : _transaction(connection, true) {}

  void commit() override { _transaction.commit(); }
  void rollback() override { _transaction.rollback(); }

 private:
  sqlpp::transaction_t<Conn> _transaction;
};

template <typename Conn, typename Conf>
class ConnectionImpl : public Connection {
 public:
  explicit ConnectionImpl(const Config& config) {
    _conn = std::move(fromConfig(config));
  }

  std::unique_ptr<Conn> fromConfig(const Config& config);

  std::unique_ptr<Transaction> transaction() override {
    return std::make_unique<TransactionImpl<Conn>>(*_conn);
  }

  void createUser(const std::string& identifier) override {
    db::helper<Conn>::createUser(*_conn, identifier);
  }

  nonstd::optional<int64_t> userIdFromIdentifier(
      const std::string& identifier) override {
    return db::helper<Conn>::userIdFromIdentifier(*_conn, identifier);
  }

  std::vector<AddressWithID> unsweptUserAddresses() override {
    return db::helper<Conn>::unsweptUserAddresses(*_conn);
  }

  std::unordered_multimap<uint64_t, std::string> tailsForUserAddresses(
      const std::vector<uint64_t>& userIds) override {
    return db::helper<Conn>::tailsForUserAddresses(*_conn, userIds);
  }

  uint64_t availableBalanceForUser(uint64_t userId) override {
    return db::helper<Conn>::availableBalanceForUser(*_conn, userId);
  }

  uint64_t createUserAddress(const common::crypto::Address& address,
                             uint64_t userId,
                             const common::crypto::UUID& uuid) override {
    return db::helper<Conn>::createUserAddress(*_conn, address, userId, uuid);
  }

  void createUserAddressBalanceEntry(
      uint64_t addressId, int64_t amount,
      nonstd::optional<common::crypto::Message> message,
      const UserAddressBalanceReason reason,
      nonstd::optional<std::string> tailHash,
      nonstd::optional<uint64_t> sweepId) override {
    db::helper<Conn>::createUserAddressBalanceEntry(
        *_conn, addressId, amount, message, reason, std::move(tailHash),
        std::move(sweepId));
  }

  void createUserAccountBalanceEntry(
      uint64_t userId, int64_t amount, const UserAccountBalanceReason reason,
      const nonstd::optional<uint64_t> fkey = {}) override {
    db::helper<Conn>::createUserAccountBalanceEntry(*_conn, userId, amount,
                                                    reason, std::move(fkey));
  }

  uint64_t createWithdrawal(
      const std::string& uuid, uint64_t userId, uint64_t amount,
      const common::crypto::Tag& tag,
      const common::crypto::Address& payoutAddress) override {
    return db::helper<Conn>::createWithdrawal(*_conn, uuid, userId, amount, tag,
                                              payoutAddress);
  }

  size_t cancelWithdrawal(const std::string& uuid) override {
    return db::helper<Conn>::cancelWithdrawal(*_conn, uuid);
  }

  size_t setWithdrawalSweep(uint64_t id, uint64_t sweepId) override {
    return db::helper<Conn>::setWithdrawalSweep(*_conn, id, sweepId);
  }

  nonstd::optional<AddressWithUUID> selectFirstUserAddress() override {
    return db::helper<Conn>::selectFirstUserAddress(*_conn);
  }

  void markUUIDAsSigned(const common::crypto::UUID& uuid) override {
    db::helper<Conn>::markUUIDAsSigned(*_conn, uuid);
  }

  std::vector<UserAccountBalanceEvent> getAllUsersAccountBalancesSinceTimePoint(
      std::chrono::system_clock::time_point newerThan) override {
    return db::helper<Conn>::getAllUsersAccountBalancesSinceTimePoint(
        *_conn, newerThan);
  }

  std::vector<Sweep> getUnconfirmedSweeps(
      const std::chrono::system_clock::time_point& olderThan) override {
    return db::helper<Conn>::getUnconfirmedSweeps(*_conn, olderThan);
  }

  void createTail(uint64_t sweepId, const std::string& hash) override {
    db::helper<Conn>::createTail(*_conn, sweepId, hash);
  }

  std::vector<SweepTail> getTailsForSweep(uint64_t sweepId) override {
    return db::helper<Conn>::getTailsForSweep(*_conn, sweepId);
  }

  void markTailAsConfirmed(const std::string& hash) override {
    db::helper<Conn>::markTailAsConfirmed(*_conn, hash);
  }

  std::vector<UserAccountBalanceEvent> getUserAccountBalances(
      uint64_t userId,
      std::chrono::system_clock::time_point newerThan) override {
    return db::helper<Conn>::getUserAccountBalances(*_conn, userId, newerThan);
  }

  std::vector<UserAddressBalanceEvent>
  getAllUserAddressesBalancesSinceTimePoint(
      std::chrono::system_clock::time_point newerThan) override {
    return db::helper<Conn>::getAllUserAddressesBalancesSinceTimePoint(
        *_conn, newerThan);
  }

  std::vector<HubAddressBalanceEvent> getAllHubAddressesBalancesSinceTimePoint(
      std::chrono::system_clock::time_point newerThan) override {
    return db::helper<Conn>::getAllHubAddressesBalancesSinceTimePoint(
        *_conn, newerThan);
  }

  std::vector<SweepEvent> getSweeps(
      std::chrono::system_clock::time_point newerThan) override {
    return db::helper<Conn>::getSweeps(*_conn, newerThan);
  }

  void insertUserTransfers(
      const std::vector<UserTransfer>& transfers) override {
    db::helper<Conn>::insertUserTransfers(*_conn, transfers);
  }

  std::map<std::string, int64_t> userIdsFromIdentifiers(
      const std::set<std::string>& identifiers) override {
    return db::helper<Conn>::userIdsFromIdentifiers(*_conn, identifiers);
  }

  std::map<uint64_t, int64_t> getTotalAmountForUsers(
      const std::set<uint64_t>& ids) override {
    return db::helper<Conn>::getTotalAmountForUsers(*_conn, ids);
  }

  std::map<uint64_t, int64_t> getTotalAmountForAddresses(
      const std::set<uint64_t>& ids) override {
    return db::helper<Conn>::getTotalAmountForAddresses(*_conn, ids);
  }

  WithdrawalInfo getWithdrawalInfoFromUUID(const std::string& uuid) override {
    return db::helper<Conn>::getWithdrawalInfoFromUUID(*_conn, uuid);
  }

  int64_t createHubAddress(const common::crypto::UUID& uuid,
                           const common::crypto::Address& address) override {
    return db::helper<Conn>::createHubAddress(*_conn, uuid, address);
  }

  void createHubAddressBalanceEntry(uint64_t hubAddress, int64_t amount,
                                    const HubAddressBalanceReason reason,
                                    uint64_t sweepId) override {
    return db::helper<Conn>::createHubAddressBalanceEntry(
        *_conn, hubAddress, amount, reason, sweepId);
  }

  uint64_t getHubAddressBalance(uint64_t hubAddress) override {
    return db::helper<Conn>::getHubAddressBalance(*_conn, hubAddress);
  }

  uint64_t getUserAddressBalance(uint64_t userAddress) override {
    return db::helper<Conn>::getUserAddressBalance(*_conn, userAddress);
  }

  int64_t createSweep(const common::crypto::Hash& bundleHash,
                      const std::string& bundleTrytes,
                      uint64_t intoHubAddress) override {
    return db::helper<Conn>::createSweep(*_conn, bundleHash, bundleTrytes,
                                         intoHubAddress);
  };

  std::vector<TransferOutput> getWithdrawalsForSweep(
      size_t max,
      const std::chrono::system_clock::time_point& olderThan) override {
    return db::helper<Conn>::getWithdrawalsForSweep(*_conn, max, olderThan);
  };

  std::vector<TransferInput> getDepositsForSweep(
      size_t max,
      const std::chrono::system_clock::time_point& olderThan) override {
    return db::helper<Conn>::getDepositsForSweep(*_conn, max, olderThan);
  };

  std::vector<TransferInput> getHubInputsForSweep(
      uint64_t requiredAmount,
      const std::chrono::system_clock::time_point& olderThan) override {
    return db::helper<Conn>::getHubInputsForSweep(*_conn, requiredAmount,
                                                  olderThan);
  }

  bool isSweepConfirmed(uint64_t sweepId) override {
    return db::helper<Conn>::isSweepConfirmed(*_conn, sweepId);
  }

  nonstd::optional<AddressInfo> getAddressInfo(
      const common::crypto::Address& address) override {
    return db::helper<Conn>::getAddressInfo(*_conn, address);
  }

  nonstd::optional<SweepEvent> getSweepByBundleHash(
      const common::crypto::Hash& bundleHash) override {
    return db::helper<Conn>::getSweepByBundleHash(*_conn, bundleHash);
  }

  nonstd::optional<SweepEvent> getSweepByWithdrawalUUID(
      const std::string& uuid) override {
    return db::helper<Conn>::getSweepByWithdrawalUUID(*_conn, uuid);
  }

  nonstd::optional<SweepDetail> getSweepDetailByBundleHash(
      const common::crypto::Hash& bundleHash) override {
    return db::helper<Conn>::getSweepDetailByBundleHash(*_conn, bundleHash);
  }

  uint64_t getTotalBalance() override {
    return db::helper<Conn>::getTotalBalance(*_conn);
  }

  void execute(const std::string& what) override { _conn->execute(what); }

 private:
  std::unique_ptr<Conn> _conn;
};

}  // namespace db
}  // namespace hub

#endif  // HUB_DB_CONNECTION_IMPL_H_
