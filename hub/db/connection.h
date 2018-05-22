// Copyright 2018 IOTA Foundation

#ifndef HUB_DB_CONNECTION_H_
#define HUB_DB_CONNECTION_H_

#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <nonstd/optional.hpp>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <unordered_map>

#include "hub/crypto/types.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/db/types.h"

namespace hub {
namespace db {

class Transaction {
 public:
  virtual ~Transaction() {}
  virtual void commit() = 0;
  virtual void rollback() = 0;
};

class Connection {
 public:
  virtual ~Connection() {}

  virtual std::unique_ptr<Transaction> transaction() = 0;
  virtual nonstd::optional<int64_t> userIdFromIdentifier(
      const std::string& identifier) = 0;
  virtual std::vector<AddressWithID> unsweptUserAddresses() = 0;
  virtual std::unordered_multimap<uint64_t, std::string> tailsForUserAddresses(
      const std::vector<uint64_t>& userIds) = 0;
  virtual uint64_t availableBalanceForUser(uint64_t userId) = 0;
  virtual void createUser(const std::string& identifier) = 0;
  virtual void createUserAddressBalanceEntry(
      uint64_t addressId, int64_t amount, const UserAddressBalanceReason reason,
      nonstd::optional<std::string> tailHash,
      nonstd::optional<uint64_t> sweepId) = 0;
  virtual uint64_t createUserAddress(const hub::crypto::Address& address,
                                     uint64_t userId,
                                     const hub::crypto::UUID& uuid) = 0;
  virtual void createUserAccountBalanceEntry(
      uint64_t userId, int64_t amount, const UserAccountBalanceReason reason,
      const nonstd::optional<uint64_t> fkey = {}) = 0;
  virtual uint64_t createWithdrawal(
      const std::string& uuid, uint64_t userId, uint64_t amount,
      const hub::crypto::Address& payoutAddress) = 0;
  virtual size_t cancelWithdrawal(const std::string& uuid) = 0;
  virtual size_t setWithdrawalSweep(uint64_t id, uint64_t sweepId) = 0;
  virtual nonstd::optional<AddressWithUUID> selectFirstUserAddress() = 0;
  virtual void markUUIDAsSigned(const hub::crypto::UUID& uuid) = 0;
  virtual std::vector<UserBalanceEvent> getUserAccountBalances(
      uint64_t userId) = 0;
  virtual std::vector<Sweep> getUnconfirmedSweeps(
      const std::chrono::system_clock::time_point& olderThan) = 0;
  virtual void createTail(uint64_t sweepId, const std::string& hash) = 0;
  virtual std::vector<std::string> getTailsForSweep(uint64_t sweepId) = 0;
  virtual void markTailAsConfirmed(const std::string& hash) = 0;
  virtual std::vector<UserBalanceEvent> getAccountBalances(
      std::chrono::system_clock::time_point newerThan) = 0;
  virtual void insertUserTransfers(
      const std::vector<UserTransfer>& transfers) = 0;
  virtual std::map<std::string, int64_t> userIdsFromIdentifiers(
      const std::set<std::string>& identifiers) = 0;
  virtual std::map<uint64_t, int64_t> getTotalAmountForUsers(
      const std::set<uint64_t>& ids) = 0;
  virtual std::map<uint64_t, int64_t> getTotalAmountForAddresses(
      const std::set<uint64_t>& ids) = 0;

  virtual WithdrawalInfo getWithdrawalInfoFromUUID(const std::string& uuid) = 0;

  virtual int64_t createHubAddress(const hub::crypto::UUID& uuid,
                                   const hub::crypto::Address& address) = 0;
  virtual void createHubAddressBalanceEntry(
      uint64_t hubAddress, int64_t amount, const HubAddressBalanceReason reason,
      uint64_t sweepId) = 0;

  virtual uint64_t getHubAddressBalance(uint64_t hubAddress) = 0;
  virtual uint64_t getUserAddressBalance(uint64_t userAddress) = 0;

  virtual int64_t createSweep(const hub::crypto::Hash& bundleHash,
                              const std::string& bundleTrytes,
                              uint64_t intoHubAddress) = 0;

  virtual std::vector<TransferOutput> getWithdrawalsForSweep(
      size_t max, const std::chrono::system_clock::time_point& olderThan) = 0;

  virtual std::vector<TransferInput> getDepositsForSweep(
      size_t max, const std::chrono::system_clock::time_point& olderThan) = 0;

  virtual std::vector<TransferInput> getHubInputsForSweep(
      uint64_t requiredAmount,
      const std::chrono::system_clock::time_point& olderThan) = 0;
  virtual bool isSweepConfirmed(uint64_t sweepId) = 0;

 private:
  friend class DBManager;
  virtual void execute(const std::string& what) = 0;
};

}  // namespace db
}  // namespace hub

#endif  // HUB_DB_CONNECTION_H_
