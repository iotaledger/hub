#include "user_address_monitor.h"

#include <iterator>
#include <vector>

#include <glog/logging.h>
#include <boost/move/move.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "hub/db/db.h"
#include "hub/db/helper.h"

using boost::adaptors::filtered;
using boost::adaptors::transformed;

namespace tags {
struct ByAddressID {};
}  // namespace tags

namespace {
using namespace boost::multi_index;

using BalanceChange = hub::service::AddressMonitor::BalanceChange;
using BalanceMap = boost::multi_index_container<
    BalanceChange,
    indexed_by<ordered_non_unique<
        tag<tags::ByAddressID>,
        member<BalanceChange, const uint64_t, &BalanceChange::addressId>>>>;
}  // namespace

namespace hub {
namespace service {

std::vector<std::tuple<uint64_t, std::string>>
UserAddressMonitor::monitoredAddresses() {
  return hub::db::unsweptUserAddresses(hub::db::DBManager::get().connection());
}

bool UserAddressMonitor::onBalancesChanged(
    std::vector<AddressMonitor::BalanceChange>& changes) {
  LOG(INFO) << "Processing changes. Total: " << changes.size();
  auto& connection = db::DBManager::get().connection();
  bool error = true;

  sqlpp::transaction_t<hub::db::Connection> transaction(connection, true);

  try {
    for (const auto& change : changes) {
      auto confirmedBundles =
          _api->getConfirmedBundlesForAddress(change.address);
      auto tails = db::tailsForAddress(connection, change.addressId);

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
              // TODO (th0br0) verify that this was a sweep. Else we've got a
              // problem.
            } else {
              db::createUserAddressBalanceEntry(
                  connection, change.addressId, tx.value,
                  db::UserAddressBalanceReason::DEPOSIT, tail, {});
            }
          }
        }
      }

      if (aggregateSum != change.delta) {
        LOG(ERROR) << "On-Tangle sum did not match expected. Expected: "
                   << change.delta << " but saw: " << aggregateSum;
        goto cleanup;
      }
    }

    error = false;
  cleanup:
    if (error) {
      transaction.rollback();
    } else {
      transaction.commit();
      return true;
    }
  } catch (sqlpp::exception& ex) {
    transaction.rollback();

    LOG(ERROR) << " Commit failed: " << ex.what();
  }

  return false;
}

}  // namespace service
}  // namespace hub
