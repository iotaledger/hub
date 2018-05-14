// Copyright 2018 IOTA Foundation

#ifndef HUB_DB_HELPER_H_
#define HUB_DB_HELPER_H_

#include <chrono>
#include <cstdint>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "hub/crypto/types.h"
#include "hub/db/db.h"
#include "hub/db/types.h"
#include "schema/schema.h"

namespace hub {
namespace db {

template <typename C>
struct helper {
  static std::optional<int64_t> userIdFromIdentifier(
      C& connection, const std::string& identifier);
  static std::vector<AddressWithID> unsweptUserAddresses(C& connection);
  static std::vector<std::string> tailsForAddress(C& connection,
                                                  uint64_t userId);
  static std::optional<uint64_t> availableBalanceForUser(C& connection,
                                                         uint64_t userId);
  static void createUser(C& connection, const std::string& identifier);
  static void createUserAddress(C& connection,
                                const hub::crypto::Address& address,
                                uint64_t userId, const hub::crypto::UUID& uuid);
  static void createUserAddressBalanceEntry(
      C& connection, uint64_t addressId, int64_t amount,
      const UserAddressBalanceReason reason,
      std::optional<std::string> tailHash, std::optional<uint64_t> sweepId);
  static void createUserAccountBalanceEntry(
      C& connection, uint64_t userId, int64_t amount,
      const UserAccountBalanceReason reason,
      const std::optional<uint64_t> fkey = {});
  static uint64_t createWithdrawal(C& connection, const std::string& uuid,
                                   uint64_t userId, uint64_t amount,
                                   const hub::crypto::Address& payoutAddress);
  static size_t cancelWithdrawal(C& connection, const std::string& uuid);
  static std::optional<AddressWithUUID> selectFirstUserAddress(C& connection);
  static void markUUIDAsSigned(C& connection, const hub::crypto::UUID& uuid);
  static std::vector<UserBalanceEvent> getUserAccountBalances(C& connection,
                                                              uint64_t userId);
  static std::vector<Sweep> getUnconfirmedSweeps(
      C& connection, const std::chrono::system_clock::time_point& olderThan);
  static void createTail(C& connection, uint64_t sweepId,
                         const std::string& hash);
  static std::vector<std::string> getTailsForSweep(C& connection,
                                                   uint64_t sweepId);
  static void markTailAsConfirmed(C& connection, const std::string& hash);
  static std::vector<UserBalanceEvent> getAccountBalances(
      C& connection, std::chrono::system_clock::time_point newerThan);
  static void insertUserTransfers(C& connection,
                                  const std::vector<UserTransfer>& transfers);
  static std::map<std::string, int64_t> userIdsFromIdentifiers(
      C& connection, const std::set<std::string>& identifiers);
  static std::map<uint64_t, int64_t> getTotalAmountForUsers(
      C& connection, const std::set<uint64_t>& ids);
  static std::map<uint64_t, int64_t> getTotalAmountForAddresses(
      C& connection, const std::set<uint64_t>& ids);
};

}  // namespace db
}  // namespace hub

#endif  // HUB_DB_HELPER_H_
