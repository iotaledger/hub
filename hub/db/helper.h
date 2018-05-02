// Copyright 2018 IOTA Foundation

#ifndef HUB_DB_HELPER_H_
#define HUB_DB_HELPER_H_

#include <cstdint>
#include <optional>
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
#include <boost/uuid/uuid.hpp>

#include "hub/db/db.h"
#include "hub/db/types.h"
#include "hub/db/uuid.h"
#include "schema/schema.h"

namespace hub {
namespace db {

std::optional<int64_t> userIdFromIdentifier(Connection& connection,
                                            const std::string& identifier);

using AddressWithID = std::tuple<uint64_t, std::string>;

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

uint64_t createWithdrawal(Connection& connection,
                          const boost::uuids::uuid& uuid, uint64_t userId,
                          uint64_t amount, const std::string& payoutAddress);

size_t cancelWithdrawal(Connection& connection, const boost::uuids::uuid& uuid);

using AddressWithUUID = std::tuple<std::string, boost::uuids::uuid>;
std::optional<AddressWithUUID> selectFirstUserAddress(Connection& connection);

void markUUIDAsSigned(Connection& connection, const boost::uuids::uuid& uuid);

struct UserBalanceEvent {
  std::chrono::system_clock::time_point timestamp;
  int64_t amount;
  UserAccountBalanceReason type;
};

std::vector<UserBalanceEvent> getUserAccountBalances(Connection& connection,
                                                     uint64_t userId);

}  // namespace db
}  // namespace hub

#endif  // HUB_DB_HELPER_H_
