// Copyright 2018 IOTA Foundation

#include "hub/service/user_address_monitor.h"

#include <iterator>
#include <vector>

#include <glog/logging.h>
#include <boost/move/move.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "hub/db/db.h"
#include "hub/db/helper.h"

using boost::adaptors::filtered;
using boost::adaptors::transformed;

namespace tags {
struct ByAddressID {};
}  // namespace tags

namespace hub {
namespace service {

std::vector<std::tuple<uint64_t, std::string>>
UserAddressMonitor::monitoredAddresses() {
  return hub::db::DBManager::get().connection().unsweptUserAddresses();
}

bool UserAddressMonitor::onBalancesChanged(
    const std::vector<AddressMonitor::BalanceChange>& changes) {
  LOG(INFO) << "Processing changes. Total: " << changes.size();
  auto& connection = db::DBManager::get().connection();
  bool error = true;

  auto transaction = connection.transaction();

  try {
    for (const auto& change : changes) {
      auto confirmedBundles =
          _api->getConfirmedBundlesForAddress(change.address);
      auto tails = connection.tailsForAddress(change.addressId);

      auto unknownTails = confirmedBundles |
                          filtered([&tails](hub::iota::Bundle const& bundle) {
                            auto& ctail = bundle[0].hash;
                            if (std::none_of(tails.cbegin(), tails.cend(),
                                             [&ctail](const std::string& tail) {
                                               return ctail == tail;
                                             })) {
                              return true;
                            }
                            return false;
                          });

      int64_t aggregateSum = 0;

      for (const auto& bundle : unknownTails) {
        auto& tail = bundle[0].hash;

        for (const auto& tx : bundle) {
          if (tx.address == change.address && tx.value) {
            aggregateSum += tx.value;

            if (tx.value < 0) {
              // TODO(th0br0) verify that this was a sweep. Else we've got a
              // problem.
            } else {
              LOG(INFO) << "Creating UserAddressBalance(" << change.addressId
                        << "," << tx.value << ",DEPOSIT," << tail << ")";

              connection.createUserAddressBalanceEntry(
                  change.addressId, tx.value,
                  db::UserAddressBalanceReason::DEPOSIT, tail, {});
            }
          }
        }
      }

      if (aggregateSum != change.delta) {
        LOG(ERROR)
            << "On-Tangle sum did not match expected. Expected (getBalances): "
            << change.delta << " observed: " << aggregateSum;
        goto cleanup;
      }

      if (!validateBalanceIsConsistent(change.address, change.addressId)) {
        goto cleanup;
      }
    }

    error = false;
  cleanup:
    if (error) {
      LOG(ERROR) << "Controlled rollback.";

      transaction->rollback();
    } else {
      transaction->commit();
      return true;
    }
  } catch (sqlpp::exception& ex) {
    LOG(ERROR) << " Commit failed: " << ex.what();

    try {
      transaction->rollback();
    } catch (const std::exception& ex) {
      LOG(ERROR) << "Rollback failed: " << ex.what();
    }
  }

  return false;
}

bool UserAddressMonitor::validateBalanceIsConsistent(const std::string& address,
                                                     uint64_t addressId) {
  auto& connection = db::DBManager::get().connection();
  const auto& iriBalances = _api->getBalances({address});
  const auto& dbBalances = connection.getTotalAmountForAddresses({addressId});

  if (iriBalances.size() != 1 || dbBalances.size() != 1) {
    LOG(ERROR) << "Could not get balance for address: \n" + address;
    return false;
  }

  auto observedBalance = iriBalances.begin()->second;
  auto expectedBalance = dbBalances.begin()->second;
  if (observedBalance < expectedBalance) {
    LOG(ERROR) << "Observed sum is less than expected. Expected (db): "
               << expectedBalance
               << " Observed (getBalances): " << observedBalance;
    return false;
  }

  return true;
}

}  // namespace service
}  // namespace hub
