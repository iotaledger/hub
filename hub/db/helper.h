/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_DB_HELPER_H_
#define HUB_DB_HELPER_H_

#include <chrono>
#include <cstdint>
#include <map>
#include <nonstd/optional.hpp>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "common/crypto/types.h"
#include "hub/db/db.h"
#include "hub/db/types.h"
#include "schema/schema.h"

namespace hub {
namespace db {

template <typename C>
struct helper {
  static nonstd::optional<int64_t> userIdFromIdentifier(
      C& connection, const std::string& identifier);
  static std::vector<AddressWithID> unsweptUserAddresses(C& connection);
  static std::unordered_multimap<uint64_t, std::string> tailsForUserAddresses(
      C& connection, const std::vector<uint64_t>& userIds);
  static uint64_t availableBalanceForUser(C& connection, uint64_t userId);
  static void createUser(C& connection, const std::string& identifier);
  static uint64_t createUserAddress(C& connection,
                                    const common::crypto::Address& address,
                                    uint64_t userId,
                                    const common::crypto::UUID& uuid);
  static void createUserAddressBalanceEntry(
      C& connection, uint64_t addressId, int64_t amount,
      nonstd::optional<common::crypto::Message> message,
      const UserAddressBalanceReason reason,
      nonstd::optional<std::string> tailHash,
      nonstd::optional<uint64_t> sweepId);
  static void createUserAccountBalanceEntry(
      C& connection, uint64_t userId, int64_t amount,
      const UserAccountBalanceReason reason,
      const nonstd::optional<uint64_t> fkey = {});
  static uint64_t createWithdrawal(
      C& connection, const std::string& uuid, uint64_t userId, uint64_t amount,
      const common::crypto::Tag& tag,
      const common::crypto::Address& payoutAddress);
  static size_t cancelWithdrawal(C& connection, const std::string& uuid);
  static size_t setWithdrawalSweep(C& connection, uint64_t id,
                                   uint64_t sweepId);

  static nonstd::optional<AddressWithUUID> selectFirstUserAddress(
      C& connection);
  static void markUUIDAsSigned(C& connection, const common::crypto::UUID& uuid);
  static std::vector<UserAccountBalanceEvent> getUserAccountBalances(
      C& connection, uint64_t userId,
      std::chrono::system_clock::time_point newerThan);

  static std::vector<UserAddressBalanceEvent>
  getAllUserAddressesBalancesSinceTimePoint(
      C& connection, std::chrono::system_clock::time_point newerThan);

  static std::vector<HubAddressBalanceEvent>
  getAllHubAddressesBalancesSinceTimePoint(
      C& connection, std::chrono::system_clock::time_point newerThan);

  static std::vector<Sweep> getUnconfirmedSweeps(
      C& connection, const std::chrono::system_clock::time_point& olderThan);
  static void createTail(C& connection, uint64_t sweepId,
                         const std::string& hash);
  static std::vector<SweepTail> getTailsForSweep(C& connection,
                                                 uint64_t sweepId);
  static void markTailAsConfirmed(C& connection, const std::string& hash);
  static std::vector<UserAccountBalanceEvent>
  getAllUsersAccountBalancesSinceTimePoint(
      C& connection, std::chrono::system_clock::time_point newerThan);
  static std::vector<SweepEvent> getSweeps(
      C& connection, std::chrono::system_clock::time_point newerThan);
  static void insertUserTransfers(C& connection,
                                  const std::vector<UserTransfer>& transfers);
  static std::map<std::string, int64_t> userIdsFromIdentifiers(
      C& connection, const std::set<std::string>& identifiers);
  static std::map<uint64_t, int64_t> getTotalAmountForUsers(
      C& connection, const std::set<uint64_t>& ids);
  static std::map<uint64_t, int64_t> getTotalAmountForAddresses(
      C& connection, const std::set<uint64_t>& ids);

  static WithdrawalInfo getWithdrawalInfoFromUUID(C& connection,
                                                  const std::string& uuid);

  static nonstd::optional<SweepEvent> getSweepByWithdrawalUUID(
      C& connection, const std::string& uuid);
  static nonstd::optional<SweepEvent> getSweepByBundleHash(
      C& connection, const common::crypto::Hash& bundleHash);
  static nonstd::optional<SweepDetail> getSweepDetailByBundleHash(
      C& connection, const common::crypto::Hash& bundleHash);

  static int64_t createHubAddress(C& connection,
                                  const common::crypto::UUID& uuid,
                                  const common::crypto::Address& address);
  static void createHubAddressBalanceEntry(C& connection, uint64_t hubAddress,
                                           int64_t amount,
                                           const HubAddressBalanceReason reason,
                                           uint64_t sweepId);
  static uint64_t getHubAddressBalance(C& connection, uint64_t hubAddress);
  static uint64_t getUserAddressBalance(C& connection, uint64_t userAddress);

  static int64_t createSweep(C& connection,
                             const common::crypto::Hash& bundleHash,
                             const std::string& bundleTrytes,
                             uint64_t intoHubAddress);

  static std::vector<TransferOutput> getWithdrawalsForSweep(
      C& connection, size_t max,
      const std::chrono::system_clock::time_point& olderThan);

  static std::vector<TransferInput> getDepositsForSweep(
      C& connection, size_t max,
      const std::chrono::system_clock::time_point& olderThan);

  static std::vector<TransferInput> getHubInputsForSweep(
      C& connection, uint64_t requiredAmount,
      const std::chrono::system_clock::time_point& olderThan);

  static nonstd::optional<AddressInfo> getAddressInfo(
      C& connection, const common::crypto::Address& address);

  static bool isSweepConfirmed(C& connection, uint64_t sweepId);

  static uint64_t getTotalBalance(C& connection);
};

}  // namespace db
}  // namespace hub

#endif  // HUB_DB_HELPER_H_
