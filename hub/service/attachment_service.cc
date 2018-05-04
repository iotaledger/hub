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

bool AttachmentService::checkSweepTailsForConfirmation(
    db::Connection& connection, const db::Sweep& sweep,
    const std::vector<std::string>& tails) {
  auto confirmedTails = _api->filterConfirmedTails(tails);

  LOG(INFO) << "Sweep: " << sweep.id << " (" << sweep.bundleHash
            << ") has: " << confirmedTails.size() << " confirmed tails.";

  if (confirmedTails.size() == 1) {
    const auto& tail = *confirmedTails.cbegin();
    LOG(INFO) << "Marking tail as confirmed: " << tail;
    db::markTailAsConfirmed(connection, tail);
    return true;
  } else if (confirmedTails.size() > 1) {
    LOG(FATAL) << "More than one confirmed tail!!!";
  }

  return false;
}

bool AttachmentService::checkForUserReattachment(
    db::Connection& connection, const db::Sweep& sweep,
    const std::vector<std::string>& knownTails) {
  auto bundleTransactionHashes =
      _api->findTransactions({}, std::vector<std::string>{sweep.bundleHash});
  auto bundleTransactions = _api->getTrytes(bundleTransactionHashes);

  // Remove non-tails or tails that we know of
  bundleTransactions.erase(
      std::remove_if(bundleTransactions.begin(), bundleTransactions.end(),
                     [&knownTails](const auto& tx) {
                       return
                           // Not a tail
                           tx.currentIndex != 0 ||
                           // Already know about this one
                           std::find(knownTails.begin(), knownTails.end(),
                                     tx.hash) != knownTails.end();
                     }),
      bundleTransactions.end());

  if (bundleTransactions.size() != 0) {
    // Third-party (i.e. user erattachments!)
    std::vector<std::string> userTails;

    std::transform(bundleTransactions.cbegin(), bundleTransactions.cend(),
                   userTails.begin(), [](const auto& ref) { return ref.hash; });

    auto confirmedTails = _api->filterConfirmedTails(userTails);
    if (confirmedTails.size() != 0) {
      // FIXME what if error logic more than one confirmed tail
      const auto& tail = *confirmedTails.cbegin();
      LOG(INFO) << "Inserting confirmed user-attached tail: " << tail;
      db::createTail(connection, sweep.id, tail);

      LOG(INFO) << "Marking tail as confirmed: " << tail;
      db::markTailAsConfirmed(connection, tail);
      return true;
    }

    // This also makes sure that we only add bundles that IRI has seen
    // fully.
    auto consistentTails = _api->filterConsistentTails(userTails);

    if (consistentTails.size() != 0) {
      for (const auto& tail : consistentTails) {
        LOG(INFO) << "Inserting UNconfirmed user-attached tail: " << tail;
        db::createTail(connection, sweep.id, tail);
      }
    }
  }
  return false;
}

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

    for (const auto& sweep : unconfirmedSweeps) {
      // 2. Get (tails, timestamp) for these sweeps
      {
        const auto sweepTails = db::getTailsForSweep(connection, sweep.id);

        // 3. Check if one of the tails is confirmed.
        if (checkSweepTailsForConfirmation(connection, sweep, sweepTails)) {
          continue;
        }

        // 4. If not, check if a user reattachment was confirmed
        checkForUserReattachment(connection, sweep, sweepTails);
      }

      // 5. If not, check if at least one of the tails per sweep is still
      //    promotable
      {
        // Requerying list of tails because `checkForUserReattachment` might
        // have added some.
        const auto sweepTails = db::getTailsForSweep(connection, sweep.id);
        auto consistentTails = _api->filterConsistentTails(sweepTails);
        // 5.1. If yes, pick most recent and promote.
        if (consistentTails.size() > 0) {
          // Get newest tail.
          auto toPromote = std::find_if(sweepTails.begin(), sweepTails.end(),
                                        [&consistentTails](const auto& tail) {
                                          return consistentTails.find(tail) !=
                                                 consistentTails.end();
                                        });

          // TODO(th0br0): promote.
        }
      }

      // 6. If not, reattach and commit tail to DB.
      // TODO(th0br0): reattach
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
