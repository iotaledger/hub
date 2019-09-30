/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include <cstdint>

#include "common/converter.h"
#include "hub/commands/converter.h"
#include "hub/commands/factory.h"
#include "hub/commands/helper.h"
#include "hub/db/helper.h"

#include "hub/commands/get_user_history.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<GetUserHistory> registrator;

boost::property_tree::ptree GetUserHistory::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  GetUserHistoryRequest req;
  GetUserHistoryReply rep;
  auto maybeUserId = request.get_optional<std::string>("userId");
  if (!maybeUserId) {
    tree.add("error",
             common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
    return tree;
  }

  req.userId = maybeUserId.value();
  req.newerThan = 0;
  auto maybeNewerThan = request.get_optional<std::uint64_t>("newerThan");
  if (maybeNewerThan) {
    req.newerThan = maybeNewerThan.value();
  }

  auto status = doProcess(&req, &rep);

  if (status != common::cmd::OK) {
    tree.add("error", common::cmd::getErrorString(status));
  } else {
    int i = 0;
    for (auto event : rep.events) {
      auto eventId = "event_" + std::to_string(i++);
      tree.add(eventId, "");
      tree.put(eventId + ".userID", event.userId);
      tree.put(eventId + ".timestamp", event.timestamp);
      tree.put(eventId + ".amount", event.amount);
      tree.put(eventId + ".reason",
               userAccountBalanceEventReasonToString(event.reason));
      tree.put(eventId + ".sweepBundleHash", event.sweepBundleHash);
      tree.put(eventId + ".withdrawalUUID", event.withdrawalUUID);
    }
  }

  return tree;
}

common::cmd::Error GetUserHistory::doProcess(
    const GetUserHistoryRequest* request,
    GetUserHistoryReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();
  uint64_t userId;

  // Get userId for identifier
  {
    auto maybeUserId = connection.userIdFromIdentifier(request->userId);
    if (!maybeUserId) {
      return common::cmd::USER_DOES_NOT_EXIST;
    }

    userId = maybeUserId.value();
  }

  {
    std::chrono::milliseconds dur(request->newerThan);
    std::chrono::time_point<std::chrono::system_clock> newerThan(dur);
    auto balances = connection.getUserAccountBalances(userId, newerThan);

    for (const auto& b : balances) {
      response->events.emplace_back(hub::cmd::UserAccountBalanceEvent{
        userId : b.userIdentifier,
        timestamp : common::timepointToUint64(b.timestamp),
        reason : userAccountBalanceEventReasonFromSql(b.reason),
        amount : b.amount,
        sweepBundleHash : b.sweepBundleHash,
        withdrawalUUID : b.withdrawalUUID
      });
    }
  }

  return common::cmd::OK;
}

}  // namespace cmd

}  // namespace hub
