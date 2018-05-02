// Copyright 2018 IOTA Foundation

#include "hub/service/attachment_service.h"

#include <glog/logging.h>
#include <algorithm>
#include <unordered_map>
#include <vector>

#include <boost/functional/hash.hpp>

#include "hub/db/helper.h"

namespace hub {
namespace service {
bool AttachmentService::doTick() {
  // We're splitting processing out to separate methods on purpose.
  // This means, that we can't really batch API requests across sweeps.
  // Shouldn't matter though as we simply assume that the IOTA node is only used
  // by Hub.

  auto& connection = db::DBManager::get().connection();
  auto tickStart = std::chrono::system_clock::now();

  sqlpp::transaction_t<hub::db::Connection> transaction(connection, true);

  try {
    auto milestone = _api->getNodeInfo().latestMilestone;

    // 1. Get Unconfirmed sweeps from database.
    auto unconfirmedSweeps = db::getUnconfirmedSweeps(connection, tickStart);
    LOG(INFO) << "Found " << unconfirmedSweeps.size() << " unconfirmed sweeps.";

    // 2. Get (tails, timestamp) for these sweeps
    std::unordered_map<db::Sweep, std::vector<db::SweepTail>> tailsBySweep;

    for (const auto& sweep : unconfirmedSweeps) {
      auto sweepTails = db::getTailsForSweep(connection, sweep.id);
      // sort sweepTails newest first.
      std::sort(sweepTails.begin(), sweepTails.end(),
                [](const auto& a, const auto& b) {
                  return a.createdAt > b.createdAt;
                });

      // 3. Check if one of the tails is confirmed.
      std::vector<std::string> tails;
      std::transform(sweepTails.cbegin(), sweepTails.cend(), tails.begin(),
                     [](const auto& tail) { return tail.hash; });
      auto confirmedTails = _api->filterConfirmedTails(tails);

      LOG(INFO) << "Sweep: " << sweep.id << " (" << sweep.bundleHash
                << ") has: " << confirmedTails.size() << " confirmed tails.";

      if (confirmedTails.size() == 1) {
        const auto& tail = *confirmedTails.cbegin();
        LOG(INFO) << "Marking tail as confirmed: " << tail;
        db::markTailAsConfirmed(connection, tail);
        continue;
      } else if (confirmedTails.size() > 1) {
        LOG(FATAL) << "More than one confirmed tail!!!";
      }

      // 4. If not, check if at least one of the tails per sweep is still
      //    promotable

      auto consistentTails = _api->filterConsistentTails(tails);
      // 4.1. If yes, pick most recent and promote.
      if (consistentTails.size() > 0) {
        // Get newest tail.
        auto newest = std::find_if(sweepTails.begin(), sweepTails.end(),
                                   [&consistentTails](const auto& tail) {
                                     return consistentTails.find(tail.hash) !=
                                            consistentTails.end();
                                   });

        // TODO promote.
      }

      // 5. If not, check if a user reattachment was confirmed
      auto bundleTransactionHashes = _api->findTransactions(
          {}, std::vector<std::string>{sweep.bundleHash});
      auto bundleTransactions = _api->getTrytes(bundleTransactionHashes);

      // Remove non-tails or tails that we know of
      bundleTransactions.erase(
          std::remove_if(bundleTransactions.begin(), bundleTransactions.end(),
                         [&tails](const auto& tx) {
                           return
                               // Not a tail
                               tx.currentIndex != 0 ||
                               // Already know about this one
                               std::find(tails.begin(), tails.end(), tx.hash) !=
                                   tails.end();
                         }),
          bundleTransactions.end());

      if (bundleTransactions.size() != 0) {
        // Third-party (i.e. user erattachments!)
        std::vector<std::string> userTails;

        std::transform(bundleTransactions.cbegin(), bundleTransactions.cend(),
                       userTails.begin(),
                       [](const auto& ref) { return ref.hash; });

        auto confirmedTails = _api->filterConfirmedTails(userTails);
        if (confirmedTails.size() != 0) {
          // FIXME what if error logic more than one confirmed tail
          const auto& tail = *confirmedTails.cbegin();
          LOG(INFO) << "Inserting confirmed user-attached tail: " << tail;
          db::createTail(connection, sweep.id, tail);

          LOG(INFO) << "Marking tail as confirmed: " << tail;
          db::markTailAsConfirmed(connection, tail);
          continue;
        }

        auto consistentTails = _api->filterConsistentTails(userTails);

        if (consistentTails.size() != 0) {
          for (const auto& tail : consistentTails) {
            LOG(INFO) << "Inserting UNconfirmed user-attached tail: " << tail;
            db::createTail(connection, sweep.id, tail);
          }
        }
      }

      // 6. If not, reattach and commit tail to DB.
      // TODO reattach
      LOG(INFO) << "Sweep " << sweep.id << " is still unconfirmed.";
    }

    transaction.commit();
  } catch (const std::exception& ex) {
    LOG(ERROR) << "Failed to commit to DB: " << ex.what();
    transaction.rollback();
  }

  return true;
}
}  // namespace service
}  // namespace hub
