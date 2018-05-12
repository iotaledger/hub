// Copyright 2018 IOTA Foundation

#ifndef HUB_DB_CONNECTION_H_
#define HUB_DB_CONNECTION_H_

#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

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
  virtual std::optional<int64_t> userIdFromIdentifier(
      const std::string& identifier) = 0;
  virtual std::vector<AddressWithID> unsweptUserAddresses() = 0;
  virtual std::vector<std::string> tailsForAddress(uint64_t userId) = 0;
  virtual std::optional<uint64_t> availableBalanceForUser(uint64_t userId) = 0;
  virtual void createUser(const std::string& identifier) = 0;
  virtual void createUserAddressBalanceEntry(
      uint64_t addressId, int64_t amount, const UserAddressBalanceReason reason,
      std::optional<std::string> tailHash, std::optional<uint64_t> sweepId) = 0;
  virtual void createUserAddress(const std::string& address, uint64_t userId,
                                 const std::string& uuid) = 0;
  virtual void createUserAccountBalanceEntry(
      uint64_t userId, int64_t amount, const UserAccountBalanceReason reason,
      const std::optional<uint64_t> fkey = {}) = 0;
  virtual uint64_t createWithdrawal(const std::string& uuid, uint64_t userId,
                                    uint64_t amount,
                                    const std::string& payoutAddress) = 0;
  virtual size_t cancelWithdrawal(const std::string& uuid) = 0;
  virtual std::optional<AddressWithUUID> selectFirstUserAddress() = 0;
  virtual void markUUIDAsSigned(const std::string& uuid) = 0;
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

 private:
  friend class DBManager;
  virtual void execute(const std::string& what) = 0;
};

}  // namespace db
}  // namespace hub

#endif  // HUB_DB_CONNECTION_H_
