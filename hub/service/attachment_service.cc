/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/service/attachment_service.h"

#include <algorithm>
#include <chrono>
#include <iterator>
#include <unordered_map>
#include <vector>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <boost/functional/hash.hpp>
#include <iota/models/bundle.hpp>
#include <iota/models/transaction.hpp>

#include "common/helpers/digest.h"
#include "hub/db/helper.h"
#include "hub/iota/pow.h"

namespace {
DEFINE_uint32(
    max_promotion_age, 30,
    "Maximum age [minutes] for a sweep's tail which allows promotion, (i.e - a "
    "tail that's older than that will not be promoted, rather sweep "
    "will be reattached)");
}  // namespace

namespace hub {
namespace service {

bool AttachmentService::checkSweepTailsForConfirmation(
    db::Connection& connection, const db::Sweep& sweep,
    const std::vector<std::string>& tails) {
  VLOG(3) << __FUNCTION__;
  auto confirmedTails = _api->filterConfirmedTails(tails, {});

  LOG(INFO) << "Sweep: " << sweep.id << " (" << sweep.bundleHash
            << ") has: " << confirmedTails.size() << " confirmed tails.";

  if (confirmedTails.empty()) {
    return false;
  }

  auto tailIt = confirmedTails.cbegin();
  LOG(INFO) << "Marking tail as confirmed: " << *tailIt;
  connection.markTailAsConfirmed(*tailIt);
  if (confirmedTails.size() > 1) {
    LOG(ERROR) << "More than one confirmed tail for sweep: " << sweep.id
               << " bundle hash: " << sweep.bundleHash;
    LOG(INFO) << "Ignored tails:";
    while (++tailIt != confirmedTails.cend()) {
      LOG(INFO) << *tailIt;
    }
  }

  return true;
}

bool AttachmentService::checkForUserReattachment(
    db::Connection& connection, const db::Sweep& sweep,
    const std::vector<std::string>& knownTails) {
  VLOG(3) << __FUNCTION__;
  auto bundleTransactionHashes = _api->findTransactions(
      {}, std::vector<std::string>{sweep.bundleHash}, {});
  auto bundleTransactions =
      _api->getTransactions(bundleTransactionHashes, false);

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

    for (const auto& ref : bundleTransactions) {
      userTails.push_back(ref.hash);
    }

    auto confirmedTails = _api->filterConfirmedTails(userTails, {});
    if (confirmedTails.size() == 1) {
      const auto& tail = *confirmedTails.cbegin();
      LOG(INFO) << "Inserting confirmed user-attached tail: " << tail;
      connection.createTail(sweep.id, tail);

      LOG(INFO) << "Marking tail as confirmed: " << tail;
      connection.markTailAsConfirmed(tail);
      return true;
    }

    if (confirmedTails.size() > 1) {
      // This should be impossible since bundle spends all funds
      // in any input address, meaning that it won't be validated after one
      // bundle has been confirmed
      LOG(FATAL) << "Bundle hash: " << sweep.bundleHash
                 << " has more than one confirmed tail";
    }

    // This also makes sure that we only add bundles that IRI has seen
    // fully.
    auto consistentTails = _api->filterConsistentTails(userTails);

    if (consistentTails.size() != 0) {
      for (const auto& tail : consistentTails) {
        LOG(INFO) << "Inserting Unconfirmed user-attached tail: " << tail;
        connection.createTail(sweep.id, tail);
      }
    }
  }
  return false;
}

void AttachmentService::reattachSweep(db::Connection& dbConnection,
                                      const iota::POWProvider& powProvider,
                                      const db::Sweep& sweep) {
  VLOG(3) << __FUNCTION__;
  auto attachedTrytes = powProvider.performPOW(sweep.trytes);
  if (attachedTrytes.empty()) {
    LOG(ERROR) << "Failed in POW for sweep with bundle's hash: "
               << sweep.bundleHash;
    return;
  }
  // Get tail hash of first tx:
  auto tailHash = iota_digest(attachedTrytes[0].c_str());
  LOG(INFO) << "Reattached sweep " << sweep.id << " as: " << tailHash;

  if (!_api->storeTransactions(attachedTrytes)) {
    std::free(tailHash);
    LOG(ERROR)
        << "Failed in \"storeTransactions\" for sweep with bundle's hash: "
        << sweep.bundleHash;
    return;
  }
  if (!_api->broadcastTransactions(attachedTrytes)) {
    std::free(tailHash);
    LOG(ERROR)
        << "Failed in \"broadcastTransactions\" for sweep with bundle's hash: "
        << sweep.bundleHash;
    return;
  }

  dbConnection.createTail(sweep.id, tailHash);

  std::free(tailHash);
}

void AttachmentService::promoteSweep(db::Connection& connection,
                                     const iota::POWProvider& powProvider,
                                     const db::Sweep& sweep,
                                     const common::crypto::Hash& tailHash) {
  VLOG(3) << __FUNCTION__;
  auto maybeToApprove = _api->getTransactionsToApprove(0, {tailHash.str()});
  if (!maybeToApprove.has_value()) {
    LOG(ERROR) << "Failed to get  transactions to approve";
    return;
  }
  auto toApprove = maybeToApprove.value();
  auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                       std::chrono::system_clock::now().time_since_epoch())
                       .count();

  IOTA::Models::Bundle bundle;
  IOTA::Models::Transaction otx;

  otx.setAddress(
      "SOME9RANDOM9HUB9BEING9REATTACHED9999999999999999999999999999999999999999"
      "999999999");
  otx.setValue(0);
  otx.setTimestamp(timestamp);

  bundle.addTransaction(otx, 1);
  bundle.addTrytes({""});
  bundle.finalize();

  const auto& tx = bundle.getTransactions()[0];

  auto attachedTrytes = powProvider.doPOW(
      {tx.toTrytes()}, toApprove.trunkTransaction, toApprove.branchTransaction);

  _api->storeTransactions(attachedTrytes);
  _api->broadcastTransactions(attachedTrytes);

  auto promotionHash = iota_digest(attachedTrytes[0].c_str());
  LOG(INFO) << "Issued promotion for sweep " << sweep.id << ": "
            << promotionHash;
  std::free(promotionHash);
}

bool AttachmentService::doTick() {
  // We're splitting processing out to separate methods on purpose.
  // This means, that we can't really batch API requests across sweeps.
  // Shouldn't matter though as we simply assume that the IOTA node is only used
  // by Hub.
  VLOG(3) << __FUNCTION__;

  auto& connection = db::DBManager::get().connection();
  auto& powProvider = iota::POWManager::get().provider();

  auto tickStart = std::chrono::system_clock::now();

  // 1. Get Unconfirmed sweeps from database.
  auto unconfirmedSweeps = connection.getUnconfirmedSweeps(tickStart);
  LOG(INFO) << "Found " << unconfirmedSweeps.size() << " unconfirmed sweeps.";

  for (const auto& sweep : unconfirmedSweeps) {
    auto transaction = connection.transaction();
    auto milestone = _api->getNodeInfo().value().latestMilestone;

    try {
      // 2. Get (tails, timestamp) for these sweeps
      const auto initialSweepInfos = connection.getTailsForSweep(sweep.id);

      std::vector<std::string> initialSweepTailHashes;
      std::transform(std::begin(initialSweepInfos), std::end(initialSweepInfos),
                     std::back_inserter(initialSweepTailHashes),
                     [](const auto& sweepTail) { return sweepTail.hash; });

      // 3. Check if one of the tails is confirmed.
      // 4. If not, check if a user reattachment was confirmed
      if (checkSweepTailsForConfirmation(connection, sweep,
                                         initialSweepTailHashes) ||
          checkForUserReattachment(connection, sweep, initialSweepTailHashes)) {
        LOG(INFO) << "Sweep " << sweep.id << " is CONFIRMED.";
      } else {
        LOG(INFO) << "Sweep " << sweep.id << " is still unconfirmed.";

        // 5. If not, check if at least one of the tails per sweep is still
        //    promotable
        // Requerying list of tails because `checkForUserReattachment` might
        // have added some.
        const auto sweepTails = connection.getTailsForSweep(sweep.id);
        std::vector<std::string> sweepTailHashes;
        std::transform(std::begin(sweepTails), std::end(sweepTails),
                       std::back_inserter(sweepTailHashes),
                       [](const auto& sweepTail) { return sweepTail.hash; });
        auto consistentTails = _api->filterConsistentTails(sweepTailHashes);
        // 5.1. If yes, pick most recent and promote.
        if (consistentTails.size() > 0) {
          // Get newest tail.
          auto toPromote = std::find_if(
              sweepTails.begin(), sweepTails.end(),
              [&consistentTails](const auto& tail) {
                return consistentTails.find(tail.hash) !=
                           consistentTails.end() &&
                       std::chrono::duration_cast<std::chrono::minutes>(
                           std::chrono::system_clock::now() - tail.createdAt)
                               .count() < FLAGS_max_promotion_age;
              });

          if (toPromote != sweepTails.end()) {
            // Promotion can fail if getTransactionsToApprove fails!
            // In this case, we just catch and reattach.
            try {
              promoteSweep(connection, powProvider, sweep,
                           common::crypto::Hash((*toPromote).hash));
            } catch (const std::exception& ex) {
              LOG(INFO) << "Promotion failed. Reattaching.";
              reattachSweep(connection, powProvider, sweep);
            }
          } else {
            reattachSweep(connection, powProvider, sweep);
          }
        } else {
          // 6. If not, reattach and commit tail to DB.
          reattachSweep(connection, powProvider, sweep);
        }
      }

      transaction->commit();
    } catch (const std::exception& ex) {
      LOG(ERROR) << "Sweep " << sweep.id
                 << " failed to commit to DB: " << ex.what();

      try {
        transaction->rollback();
      } catch (const std::exception& ex) {
        LOG(ERROR) << "Sweep " << sweep.id << " rollback failed: " << ex.what();
      }
    }
  }

  return true;
}
}  // namespace service
}  // namespace hub
