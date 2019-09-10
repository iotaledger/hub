/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

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
#include <unordered_map>
#include <utility>
#include <vector>

#include "common/crypto/types.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/db/types.h"

namespace hub {
namespace db {

/// Transaction abstract class.
/// Represents a single transactional operation to the database
/// Subclasses will implement the appropriate behaviour per database type
class Transaction {
 public:
  virtual ~Transaction() {}
  virtual void commit() = 0;
  virtual void rollback() = 0;
};

/// Connection abstract class.
/// Provides a high level interface to the database.
class Connection {
 public:
  /// Destructor
  virtual ~Connection() {}

  /// Create a transaction context compatible with the current database type
  /// @return Transaction - a transaction context
  virtual std::unique_ptr<Transaction> transaction() = 0;

  /// @param[in] identifier - a std::string user identifier
  /// @return int64_t - a user id or null if no user exists for the identifier
  virtual nonstd::optional<int64_t> userIdFromIdentifier(
      const std::string& identifier) = 0;

  /// Find all the user addresses that haven't been swept yet
  /// @return std::vector - a list of non swept AddressWithID
  virtual std::vector<AddressWithID> unsweptUserAddresses() = 0;

  /// Find all the user addresses that haven't been swept yet
  /// @param[in] userIds - a vector of userIds to query
  /// @return std::unordered_multimap - a map of userId to addresses
  virtual std::unordered_multimap<uint64_t, std::string> tailsForUserAddresses(
      const std::vector<uint64_t>& userIds) = 0;

  /// Get the available balance for a user
  /// @param[in] userId - the DB userId
  /// @return uint64_t - Available balance or 0 if none
  virtual uint64_t availableBalanceForUser(uint64_t userId) = 0;

  /// Inserts a new user in the database
  /// @param[in] identifier - a std::string user identifier
  virtual void createUser(const std::string& identifier) = 0;

  /// Inserts a new user address balance entry in the database
  /// @param[in] addressId - a uint64_t, the id of the address in the database
  /// @param[in] amount - a int64_t, the balance amount to store
  /// @param[in] reason - a UserAddressBalanceReason, the reason
  /// @param[in] tailHash - tailHash of the correpsonding transaction
  /// @param[in] sweepId - if the new balance is the result of a sweep
  virtual void createUserAddressBalanceEntry(
      uint64_t addressId, int64_t amount,
      nonstd::optional<common::crypto::Message> message,
      const UserAddressBalanceReason reason,
      nonstd::optional<std::string> tailHash,
      nonstd::optional<uint64_t> sweepId) = 0;

  /// Inserts a new user address entry in the database
  /// @param[in] address - a common::crypto::Address, an IOTA address
  /// @param[in] userId - the user id in the database
  /// @param[in] uuid - a common::crypto::UUID
  /// @return uint64_t - The id of the new address in the database
  virtual uint64_t createUserAddress(const common::crypto::Address& address,
                                     uint64_t userId,
                                     const common::crypto::UUID& uuid) = 0;

  /// Create a new user account balance entry in the database
  /// @param[in] userId - the user id in the database
  /// @param[in] amount - a int64_t, the amount to withdraw
  /// @param[in] reason - a UserAddressBalanceReason, the reason
  /// @param[in] fkey - the key used to create a new IOTA address
  virtual void createUserAccountBalanceEntry(
      uint64_t userId, int64_t amount, const UserAccountBalanceReason reason,
      const nonstd::optional<uint64_t> fkey = {}) = 0;

  /// Create a withdrawal entry for the user at from given address
  /// @param[in] uuid - the common::crypto::UUID identifier of the withdrawal
  /// @param[in] userId - the user id in the database
  /// @param[in] amount - a int64_t, the amount to withdraw
  /// @param[in] tag - the tag to use for the withdrawal
  /// @param[in] payoutAddress - a common::crypto::Address, the IOTA to address
  /// @return uint64_t - 1 if successful
  virtual uint64_t createWithdrawal(
      const std::string& uuid, uint64_t userId, uint64_t amount,
      const common::crypto::Tag& tag,
      const common::crypto::Address& payoutAddress) = 0;

  /// Cancel a withdrawal operation (if not already processed)
  /// @param[in] uuid - the common::crypto::UUID identifier of the withdrawal
  /// @return size_t - 1 if successful
  virtual size_t cancelWithdrawal(const std::string& uuid) = 0;

  /// Cancel a withdrawal operation (if not already processed)
  /// @param[in] id - the database id of the withdrawal in the database
  /// @param[in] sweepId - the sweepId
  /// @return size_t - 1 if successful
  virtual size_t setWithdrawalSweep(uint64_t id, uint64_t sweepId) = 0;

  /// Returns the first available UserAddress from the database
  /// @return AddressWithUUID - a UUID,address pair if any, empty if table empty
  virtual nonstd::optional<AddressWithUUID> selectFirstUserAddress() = 0;

  /// Mark a given UUID as signed
  /// @param[in] uuid - the common::crypto::UUID to mark as signed
  virtual void markUUIDAsSigned(const common::crypto::UUID& uuid) = 0;

  /// Get a list of account balances for a user
  /// @param[in] userId - the user id in the database
  /// @param[in] newerThan - the start point in time
  /// @return std::vector - a list of UserAccountBalanceEvent for this user
  virtual std::vector<UserAccountBalanceEvent> getUserAccountBalances(
      uint64_t userId, std::chrono::system_clock::time_point newerThan) = 0;

  /// Get a list of unconfirmed sweeps
  /// @param[in] olderThan - the cutoff point in time
  /// @return std::vector - a list of unconfirmed Sweep older than olderThan
  virtual std::vector<Sweep> getUnconfirmedSweeps(
      const std::chrono::system_clock::time_point& olderThan) = 0;

  /// Create a tail for a given sweep
  /// @param[in] sweepId - the id of the sweep for which to create a tail
  /// @param[in] hash - the tail hash
  virtual void createTail(uint64_t sweepId, const std::string& hash) = 0;

  /// Get a list of tails for a sweep
  /// @param[in] sweepId - the id of the sweep for which to create a tail
  /// @return std::vector - a list of SweepTail-s
  virtual std::vector<SweepTail> getTailsForSweep(uint64_t sweepId) = 0;

  /// Mark a sweep tail as confirmed
  /// @param[in] hash - the tail hash
  virtual void markTailAsConfirmed(const std::string& hash) = 0;

  /// Get a list of account balances from a point in time
  /// @param[in] newerThan - the start point in time
  /// @return std::vector - a list of UserAccountBalanceEvent
  virtual std::vector<UserAccountBalanceEvent>
  getAllUsersAccountBalancesSinceTimePoint(
      std::chrono::system_clock::time_point newerThan) = 0;

  /// Get a list of user address balances from a point in time
  /// @param[in] newerThan - the start point in time
  /// @return std::vector - a list of UserAddressBalanceEvent
  virtual std::vector<UserAddressBalanceEvent>
  getAllUserAddressesBalancesSinceTimePoint(
      std::chrono::system_clock::time_point newerThan) = 0;

  /// Get a list of hub address balances from a point in time
  /// @param[in] newerThan - the start point in time
  /// @return std::vector - a list of HubAddressBalanceEvent
  virtual std::vector<HubAddressBalanceEvent>
  getAllHubAddressesBalancesSinceTimePoint(
      std::chrono::system_clock::time_point newerThan) = 0;

  /// Gets a list of sweeps with withdrawals from a point in time
  /// @param[in] newerThan - the start point in time
  /// @return std::vector - a list of SweepEvent
  virtual std::vector<SweepEvent> getSweeps(
      std::chrono::system_clock::time_point newerThan) = 0;

  /// Inserts new UserAccountBalance records from batch of transfers
  /// @param[in] transfers - a list of UserTransfer
  virtual void insertUserTransfers(
      const std::vector<UserTransfer>& transfers) = 0;

  /// Gets database userIds from user identifiers
  /// @param[in] identifiers - a list of user identifiers
  /// @return std::vector - a list of pairs identifier/userId
  virtual std::map<std::string, int64_t> userIdsFromIdentifiers(
      const std::set<std::string>& identifiers) = 0;

  /// Gets current balances for a list of users
  /// @param[in] ids - a list of userIds
  /// @return std::vector - a list of pairs UserAccountId/balance
  virtual std::map<uint64_t, int64_t> getTotalAmountForUsers(
      const std::set<uint64_t>& ids) = 0;

  /// Gets current ammount for a list of user addresses
  /// @param[in] ids - a list of UserAddressId
  /// @return std::vector - a list of pairs UserAddressId/amount
  virtual std::map<uint64_t, int64_t> getTotalAmountForAddresses(
      const std::set<uint64_t>& ids) = 0;

  /// Gets info for a given withdrawal operation
  /// @param[in] uuid - the common::crypto::UUID identifier of the withdrawal
  /// @return WithdrawalInfo - info on the withdrawal
  virtual WithdrawalInfo getWithdrawalInfoFromUUID(const std::string& uuid) = 0;

  /// Create a new hub address from a UUID and IOTA address
  /// @param[in] uuid - a common::crypto::UUID
  /// @param[in] address - an address
  /// @return int64_t - 1 on success
  virtual int64_t createHubAddress(const common::crypto::UUID& uuid,
                                   const common::crypto::Address& address) = 0;

  /// Create a new hub address balance record with a specific amount
  /// @param[in] hubAddress - the hub address id in the database
  /// @param[in] amount - the amount
  /// @param[in] reason - a UserAddressBalanceReason, the reason
  /// @param[in] sweepId - the sweepId if the creation results from a sweep
  virtual void createHubAddressBalanceEntry(
      uint64_t hubAddress, int64_t amount, const HubAddressBalanceReason reason,
      uint64_t sweepId) = 0;

  /// Get hub address balance
  /// @param[in] hubAddress - the hub address id in the database
  /// @return int64_t - the balance
  virtual uint64_t getHubAddressBalance(uint64_t hubAddress) = 0;

  /// Get user address balance
  /// @param[in] userAddress - the user address id in the database
  /// @return int64_t - the balance
  virtual uint64_t getUserAddressBalance(uint64_t userAddress) = 0;

  /// Create a new sweep
  /// @param[in] bundleHash - the hash of the bundle for which a sweep is
  /// created
  /// @param[in] bundleTrytes - the serialized bundle in trytes
  /// @param[in] intoHubAddress - the hub address for remainder if any
  /// @return int64_t - the database id of the new sweep
  virtual int64_t createSweep(const common::crypto::Hash& bundleHash,
                              const std::string& bundleTrytes,
                              uint64_t intoHubAddress) = 0;

  /// Return a list of withdrawals up to a point in time
  /// @param[in] max - max number of withdrawals to return
  /// @param[in] olderThan - the cutoff point in time
  /// @return std::vector - a list of TransferOutput
  virtual std::vector<TransferOutput> getWithdrawalsForSweep(
      size_t max, const std::chrono::system_clock::time_point& olderThan) = 0;

  /// Return a list of deposits up to a point in time
  /// @param[in] max - max number of deposits to return
  /// @param[in] olderThan - the cutoff point in time
  /// @return std::vector - a list of TransferInput
  virtual std::vector<TransferInput> getDepositsForSweep(
      size_t max, const std::chrono::system_clock::time_point& olderThan) = 0;

  /// Return a list of hub inputs up to a point in time
  /// @param[in] requiredAmount - the required total amount to obtain
  /// @param[in] olderThan - the cutoff point in time
  /// @return std::vector - a list of TransferInput
  virtual std::vector<TransferInput> getHubInputsForSweep(
      uint64_t requiredAmount,
      const std::chrono::system_clock::time_point& olderThan) = 0;

  /// @param[in] sweepId - the sweepId if the creation results from a sweep
  /// @return bool - true if sweep is confirmed
  virtual bool isSweepConfirmed(uint64_t sweepId) = 0;

  /// Provides information on an address if available
  /// @param[in] address - the address to provide info on
  /// @return nonstd::optional<AddressInfo> - the information if available
  virtual nonstd::optional<AddressInfo> getAddressInfo(
      const common::crypto::Address& address) = 0;

  /// Looks up a sweep by a withdrawal UUID
  /// @param[in] uuid - the withdrawal UUID to use for the lookup.
  /// @return nonstd::optional<SweepEvent> - the sweep if found
  virtual nonstd::optional<SweepEvent> getSweepByWithdrawalUUID(
      const std::string& uuid) = 0;

  /// Looks up a sweep by bundle hash
  /// @param[in] bundleHash - the bundle hash to use for the lookup.
  /// @return nonstd::optional<SweepEvent> - the sweep if found
  virtual nonstd::optional<SweepEvent> getSweepByBundleHash(
      const common::crypto::Hash& bundleHash) = 0;

  /// Looks up a sweep by bundle hash
  /// @param[in] bundleHash - the bundle hash to use for the lookup.
  /// @return nonstd::optional<SweepDetail> - the sweep details  if found
  virtual nonstd::optional<SweepDetail> getSweepDetailByBundleHash(
      const common::crypto::Hash& bundleHash) = 0;

  /// Provides the total amount of user funds currently managed by the Hub
  /// @return uint64_t - the total user account balance
  virtual uint64_t getTotalBalance() = 0;

 private:
  friend class DBManager;

  virtual void execute(const std::string& what) = 0;
};

}  // namespace db
}  // namespace hub

#endif  // HUB_DB_CONNECTION_H_
