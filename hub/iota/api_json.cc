// Copyright 2018 IOTA Foundation

#include "hub/iota/api_json.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <glog/logging.h>
#include <boost/move/move.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/combine.hpp>
#include <nlohmann/json.hpp>
#include "common/helpers/digest.h"
#include "common/model/transaction.h"

using json = nlohmann::json;
using boost::adaptors::filtered;
using boost::adaptors::transformed;
using boost::adaptors::uniqued;

namespace hub {
namespace iota {

bool IotaJsonAPI::isNodeSolid() {
  auto ni = getNodeInfo();

  if (ni.latestMilestoneIndex != ni.latestSolidMilestoneIndex) {
    return false;
  }

  auto tx = getTrytes({ni.latestMilestone})[0];
  auto now = std::chrono::system_clock::now();

  if ((now - tx.timestamp) > std::chrono::seconds(300)) {
    return false;
  }

  return true;
}

NodeInfo IotaJsonAPI::getNodeInfo() {
  json req;
  req["command"] = "getNodeInfo";

  // TODO(th0br0) proper failure mechanism
  auto response = post(std::move(req)).value();

  return {response["latestMilestone"], response["latestMilestoneIndex"],
          response["latestSolidSubtangleMilestoneIndex"]};
}

std::unordered_map<std::string, uint64_t> IotaJsonAPI::getBalances(
    const std::vector<std::string>& addresses) {
  json req;
  req["command"] = "getBalances";
  req["addresses"] = addresses;
  req["threshold"] = 100;

  auto maybeResponse = post(std::move(req));

  if (!maybeResponse) {
    LOG(INFO) << __FUNCTION__ << " request failed.";
    return {};
  }

  auto& response = maybeResponse.value();
  auto balances = response["balances"].get<std::vector<std::string>>();

  std::unordered_map<std::string, uint64_t> result;

  for (const auto& tup : boost::combine(addresses, balances)) {
    std::string add, bal;
    boost::tie(add, bal) = tup;
    result.emplace(std::move(add), std::stoull(bal));
  }

  return result;
}

std::vector<std::string> IotaJsonAPI::findTransactions(
    std::optional<std::vector<std::string>> addresses,
    std::optional<std::vector<std::string>> bundles) {
  if (!addresses && !bundles) {
    return {};
  }

  json req;
  req["command"] = "findTransactions";
  if (addresses) {
    req["addresses"] = std::move(addresses.value());
  }
  if (bundles) {
    req["bundles"] = std::move(bundles.value());
  }

  auto maybeResponse = post(std::move(req));

  if (!maybeResponse) {
    LOG(INFO) << __FUNCTION__ << " request failed.";
    return {};
  }

  return maybeResponse.value()["hashes"].get<std::vector<std::string>>();
}

std::vector<Transaction> IotaJsonAPI::getTrytes(
    const std::vector<std::string>& hashes) {
  json req;
  req["command"] = "getTrytes";
  req["hashes"] = hashes;

  auto maybeResponse = post(std::move(req));
  if (!maybeResponse) {
    LOG(INFO) << __FUNCTION__ << " request failed.";
    return {};
  }

  auto trytes = maybeResponse.value()["trytes"].get<std::vector<std::string>>();

  std::vector<Transaction> txs;
  iota_transaction_t tx = transaction_new();
  std::chrono::system_clock::time_point epoch;

  boost::copy(
      trytes | transformed([&tx,
                            &epoch](const std::string& trytes) -> Transaction {
        transaction_deserialize_from_trytes(
            tx, reinterpret_cast<const tryte_t*>(trytes.c_str()));

        // We could also rely on the ordering of the hashes argument here.
        auto hash = iota_digest(trytes.c_str());
        std::string sHash = std::string(reinterpret_cast<char*>(hash), 81);
        auto address = transaction_address(tx);
        std::string sAddress =
            std::string(reinterpret_cast<char*>(address), 81);
        auto bundle = transaction_bundle(tx);
        std::string sBundle = std::string(reinterpret_cast<char*>(bundle), 81);
        auto trunk = transaction_trunk(tx);
        std::string sTrunk = std::string(reinterpret_cast<char*>(trunk), 81);

        std::chrono::seconds sinceEpoch(transaction_timestamp(tx));

        return {sHash,
                sAddress,
                transaction_value(tx),
                epoch + sinceEpoch,
                transaction_current_index(tx),
                transaction_last_index(tx),
                sBundle,
                sTrunk};
      }),
      boost::back_move_inserter(txs));

  transaction_free(tx);

  return txs;
}

std::vector<Bundle> IotaJsonAPI::getConfirmedBundlesForAddress(
    const std::string& address) {
  // 1. Get all transactions for address [findTransactions, getTrytes]
  auto txHashes = findTransactions(std::vector<std::string>{address}, {});
  auto transactions = getTrytes(txHashes);

  // 2. Filter unique bundles from these []
  std::vector<std::string> bundles;
  boost::copy(transactions | transformed([](const Transaction& tx) {
                return tx.bundleHash;
              }) | uniqued,
              boost::back_move_inserter(bundles));

  // 3. Materialise all bundles [findTransactions, getTrytes]
  txHashes = findTransactions({}, bundles);
  transactions = getTrytes(txHashes);

  // 4. Filter unconfirmed bundles [getNodeInfo, getInclusionStates]
  std::vector<std::string> tails;
  boost::copy(transactions | filtered([](const Transaction& tx) {
                return tx.currentIndex == 0;
              }) | transformed([](const Transaction& tx) { return tx.hash; }),
              boost::back_move_inserter(tails));

  auto confirmedTails = filterConfirmedTails(tails, {});

  std::vector<Bundle> confirmedBundles;
  std::unordered_map<std::string, Transaction> transactionsByHash;

  for (auto& tx : transactions) {
    transactionsByHash.emplace(tx.hash, std::move(tx));
  }

  for (const std::string& tail : confirmedTails) {
    Bundle bundle;
    auto tx = std::move(transactionsByHash[std::move(tail)]);

    while (tx.currentIndex != tx.lastIndex) {
      auto& trunk = tx.trunk;
      auto& next = transactionsByHash[trunk];

      bundle.push_back(std::move(tx));
      tx = std::move(next);
    }
    bundle.push_back(std::move(tx));
    confirmedBundles.push_back(std::move(bundle));
  }

  return confirmedBundles;
}

std::unordered_set<std::string> IotaJsonAPI::filterConfirmedTails(
    const std::vector<std::string>& tails,
    const std::optional<std::string>& reference) {
  json req;
  req["command"] = "getInclusionStates";
  req["transactions"] = tails;

  if (reference.has_value()) {
    req["tips"] = std::vector<std::string>{reference.value()};
  } else {
    auto ni = getNodeInfo();
    req["tips"] = std::vector<std::string>{ni.latestMilestone};
  }

  auto maybeResponse = post(std::move(req));

  if (!maybeResponse) {
    LOG(INFO) << __FUNCTION__ << " request failed.";
    return {};
  }

  auto& response = maybeResponse.value();
  auto states = response["states"].get<std::vector<bool>>();

  std::unordered_set<std::string> confirmedTails;

  for (auto const& tpl : boost::combine(tails, states)) {
    if (tpl.get<1>()) {
      confirmedTails.emplace(tpl.get<0>());
    }
  }

  return confirmedTails;
}

std::pair<std::string, std::string> IotaJsonAPI::getTransactionsToApprove(
    size_t depth, const std::optional<std::string>& reference) {
  json req;

  req["command"] = "getTransactionsToApprove";
  req["depth"] = depth;

  if (reference.has_value()) {
    req["reference"] = reference.value();
  }

  auto maybeResponse = post(std::move(req));

  if (!maybeResponse) {
    LOG(INFO) << __FUNCTION__ << " request failed.";
    return {};
  }

  auto& response = maybeResponse.value();

  return {response["trunkTransaction"], response["branchTransaction"]};
}

std::vector<std::string> IotaJsonAPI::attachToTangle(
    const std::string& trunkTransaction, const std::string& branchTransaction,
    size_t minWeightMagnitude, const std::vector<std::string>& trytes) {
  json req;
  req["command"] = "attachToTangle";
  req["trunkTransaction"] = trunkTransaction;
  req["branchTransaction"] = branchTransaction;
  req["minWeightMagnitude"] = minWeightMagnitude;

  // FIXME(th0br0) should decode trytes and not trust input ordering
  std::vector<std::string> localTrytes = trytes;
  std::reverse(std::begin(localTrytes), std::end(localTrytes));
  req["trytes"] = json(localTrytes);

  auto maybeResponse = post(std::move(req));

  if (!maybeResponse) {
    LOG(INFO) << __FUNCTION__ << " request failed.";
    return {};
  }

  return maybeResponse.value()["trytes"].get<std::vector<std::string>>();
}

bool IotaJsonAPI::storeTransactions(const std::vector<std::string>& trytes) {
  json req;
  req["command"] = "storeTransactions";
  req["trytes"] = trytes;

  auto maybeResponse = post(std::move(req));
  return maybeResponse.has_value();
}

bool IotaJsonAPI::broadcastTransactions(
    const std::vector<std::string>& trytes) {
  json req;
  req["command"] = "broadcastTransactions";
  req["trytes"] = trytes;

  auto maybeResponse = post(std::move(req));
  return maybeResponse.has_value();
}

std::unordered_set<std::string> IotaJsonAPI::filterConsistentTails(
    const std::vector<std::string>& tails) {
  // TODO(th0br0): implementation
  return {};
}

}  // namespace iota
}  // namespace hub
