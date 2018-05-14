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

#include <sqlpp11/functions.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/schema.h>
#include <sqlpp11/select.h>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/transaction.h>
#include <sqlpp11/update.h>

#include "hub/crypto/types.h"
#include "hub/db/db.h"
#include "hub/db/types.h"
#include "schema/schema.h"

namespace hub {
namespace db {

std::optional<int64_t> userIdFromIdentifier(Connection& connection,
                                            const std::string& identifier);

std::vector<AddressWithID> unsweptUserAddresses(Connection& connection);

std::vector<std::string> tailsForAddress(Connection& connection,
                                         uint64_t userId);

std::optional<uint64_t> availableBalanceForUser(Connection& connection,
                                                uint64_t userId);

void createUserAddressBalanceEntry(Connection& connection, uint64_t addressId,
                                   int64_t amount,
                                   const UserAddressBalanceReason reason,
                                   std::optional<std::string> tailHash,
                                   std::optional<uint64_t> sweepId);

void createUserAccountBalanceEntry(Connection& connection, uint64_t userId,
                                   int64_t amount,
                                   const UserAccountBalanceReason reason,
                                   const std::optional<uint64_t> fkey = {});

uint64_t createWithdrawal(Connection& connection, const std::string& uuid,
                          uint64_t userId, uint64_t amount,
                          const hub::crypto::Address& payoutAddress);

size_t cancelWithdrawal(Connection& connection, const std::string& uuid);

std::optional<AddressWithUUID> selectFirstUserAddress(Connection& connection);

void markUUIDAsSigned(Connection& connection, const hub::crypto::UUID& uuid);

std::vector<UserBalanceEvent> getUserAccountBalances(Connection& connection,
                                                     uint64_t userId);

std::vector<Sweep> getUnconfirmedSweeps(
    Connection& connection,
    const std::chrono::system_clock::time_point& olderThan);

void createTail(Connection& connection, uint64_t sweepId,
                const std::string& hash);

std::vector<std::string> getTailsForSweep(Connection& connection,
                                          uint64_t sweepId);

void markTailAsConfirmed(Connection& connection, const std::string& hash);

std::vector<UserBalanceEvent> getAccountBalances(
    Connection& connection, std::chrono::system_clock::time_point newerThan);

void insertUserTransfers(Connection& connection,
                         std::vector<UserTransfer> transfers);

std::map<std::string, int64_t> userIdsFromIdentifiers(
    Connection& connection, const std::set<std::string>& identifiers);

std::map<uint64_t, int64_t> getTotalAmountForUsers(
    Connection& connection, const std::set<uint64_t>& ids);

}  // namespace db
}  // namespace hub

#endif  // HUB_DB_HELPER_H_
