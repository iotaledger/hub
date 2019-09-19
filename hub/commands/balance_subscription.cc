/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/balance_subscription.h"

#include <chrono>
#include <functional>
#include <sstream>
#include <thread>
#include <utility>

#include "common/stats/session.h"
#include "hub/commands/converter.h"
#include "hub/commands/factory.h"
#include "hub/commands/helper.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "schema/schema.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<BalanceSubscription> registrator;

boost::property_tree::ptree BalanceSubscription::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  BalanceSubscriptionRequest req;
  std::vector<BalanceEvent> events;
  req.newerThan = 0;
  auto maybeNewerThan = request.get_optional<std::string>("newerThan");
  if (maybeNewerThan) {
    std::istringstream iss(maybeNewerThan.value());
    iss >> req.newerThan;
  }

  auto status = doProcess(&req, &events);

  if (status != common::cmd::OK) {
    tree.add("error", common::cmd::errorToStringMap.at(status));
  } else {
    int i = 0;
    for (auto event : events) {
      if (auto pval =
              std::get_if<cmd::UserAccountBalanceEvent>(&event.getVariant())) {
        auto eventId = "event_" + std::to_string(i++);
        tree.add(eventId, "");
        tree.put(eventId + ".type", "USER_ACCOUNT");
        tree.put(eventId + ".reason",
                 userAccountBalanceEventTypeToString(pval->type));
        tree.put(eventId + ".userId", std::move(pval->userId));
        std::stringstream ss;
        ss << pval->timestamp;
        tree.put(eventId + ".timestamp", ss.str());
        tree.put(eventId + ".sweepBundleHash",
                 std::move(pval->sweepBundleHash));
        tree.put(eventId + ".withdrawalUuid", std::move(pval->withdrawalUUID));

      } else if (auto pval = std::get_if<cmd::UserAddressBalanceEvent>(
                     &event.getVariant())) {
        auto eventId = "event_" + std::to_string(i++);
        tree.add(eventId, "");
        tree.put(eventId + ".type", "USER_ADDRESS");
        tree.put(eventId + ".reason",
                 userAddressBalanceEventTypeToString(pval->type));
        tree.put(eventId + ".userId", std::move(pval->userId));
        std::stringstream ss;
        ss << pval->timestamp;
        tree.put(eventId + ".timestamp", ss.str());
        tree.put(eventId + ".userAddress", std::move(pval->userAddress));
        tree.put(eventId + ".hash", std::move(pval->hash));
        tree.put(eventId + ".message", std::move(pval->message));

      } else if (auto pval = std::get_if<cmd::HubAddressBalanceEvent>(
                     &event.getVariant())) {
        auto eventId = "event_" + std::to_string(i++);
        tree.add(eventId, "");
        tree.put(eventId + ".type", "HUB_ADDRESS");
        tree.put(eventId + ".hubAddress", std::move(pval->hubAddress));
        tree.put(eventId + ".reason",
                 hubAddressBalanceTypeToString(pval->type));
        std::stringstream ss;
        ss << pval->timestamp;
        tree.put(eventId + ".timestamp", ss.str());
        ss.clear();
        ss << pval->amount;
        tree.put(eventId + ".amount", pval->amount);
        tree.put(eventId + ".sweepBundleHash",
                 std::move(pval->sweepBundleHash));
      }
    }
  }

  return tree;
}

common::cmd::Error BalanceSubscription::doProcess(
    const BalanceSubscriptionRequest* request,
    std::vector<BalanceEvent>* events) noexcept {
  std::chrono::milliseconds dur(request->newerThan);
  std::chrono::time_point<std::chrono::system_clock> newerThan(dur);

  auto userAccountBalances =
      getAllUsersAccountBalancesSinceTimePoint(newerThan);

  for (auto& b : userAccountBalances) {
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                             b.timestamp.time_since_epoch())
                             .count();

    events->emplace_back(UserAccountBalanceEvent{
        .userId = std::move(b.userIdentifier),
        .timestamp = timestamp,
        .type = userAccountBalanceEventTypeFromSql(b.type),
        .amount = b.amount,
        .sweepBundleHash = b.sweepBundleHash,
        .withdrawalUUID = b.withdrawalUUID});
  }

  auto userAddressBalances =
      getAllUserAddressesBalancesSinceTimePoint(newerThan);

  for (auto& b : userAddressBalances) {
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                             b.timestamp.time_since_epoch())
                             .count();
    events->emplace_back(UserAddressBalanceEvent{
        .userId = std::move(b.userIdentifier),
        .userAddress = b.userAddress,
        .amount = b.amount,
        .type = userAddressBalanceEventTypeFromSql(b.reason),
        .hash = std::move(b.hash),
        .timestamp = timestamp,
        .message = std::move(b.message)});
  }

  auto hubAddressBalances = getAllHubAddressesBalancesSinceTimePoint(newerThan);

  for (auto& b : hubAddressBalances) {
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                             b.timestamp.time_since_epoch())
                             .count();
    events->emplace_back(HubAddressBalanceEvent{
        .hubAddress = std::move(b.hubAddress),
        .amount = b.amount,
        .type = hubAddressBalanceTypeFromSql(b.reason),
        .sweepBundleHash = std::move(b.sweepBundleHash),
        .timestamp = timestamp,
    });
  }

  return common::cmd::OK;
}

std::vector<db::UserAccountBalanceEvent>
BalanceSubscription::getAllUsersAccountBalancesSinceTimePoint(
    std::chrono::system_clock::time_point newerThan) {
  return db::DBManager::get()
      .connection()
      .getAllUsersAccountBalancesSinceTimePoint(newerThan);
}

std::vector<db::UserAddressBalanceEvent>
BalanceSubscription::getAllUserAddressesBalancesSinceTimePoint(
    std::chrono::system_clock::time_point newerThan) {
  return db::DBManager::get()
      .connection()
      .getAllUserAddressesBalancesSinceTimePoint(newerThan);
}

std::vector<db::HubAddressBalanceEvent>
BalanceSubscription::getAllHubAddressesBalancesSinceTimePoint(
    std::chrono::system_clock::time_point newerThan) {
  return db::DBManager::get()
      .connection()
      .getAllHubAddressesBalancesSinceTimePoint(newerThan);
}

}  // namespace cmd
}  // namespace hub
