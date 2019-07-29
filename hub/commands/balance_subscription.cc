/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/balance_subscription.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include <utility>

#include "common/stats/session.h"
#include "hub/commands/helper.h"
#include "hub/commands/proto_sql_converter.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "proto/hub.grpc.pb.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

namespace hub {
namespace cmd {

grpc::Status BalanceSubscription::doProcess(
    const hub::rpc::BalanceSubscriptionRequest* request,
    grpc::ServerWriterInterface<hub::rpc::BalanceEvent>* writer) noexcept {
  std::chrono::milliseconds dur(request->newerthan());
  std::chrono::time_point<std::chrono::system_clock> newerThan(dur);

  auto userAccountBalances =
      getAllUsersAccountBalancesSinceTimePoint(newerThan);

  for (auto& b : userAccountBalances) {
    hub::rpc::BalanceEvent event;
    hub::rpc::UserAccountBalanceEvent* userAccountEvent =
        new hub::rpc::UserAccountBalanceEvent();
    userAccountEvent->set_userid(std::move(b.userIdentifier));
    userAccountEvent->set_timestamp(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            b.timestamp.time_since_epoch())
            .count());
    userAccountEvent->set_amount(b.amount);
    userAccountEvent->set_type(userAccountBalanceEventTypeFromSql(b.type));
    userAccountEvent->set_sweepbundlehash(std::move(b.sweepBundleHash));
    userAccountEvent->set_withdrawaluuid(std::move(b.withdrawalUUID));
    event.set_allocated_useraccountevent(userAccountEvent);
    if (!writer->Write(event)) {
      return grpc::Status::CANCELLED;
    }
  }

  auto userAddressBalances =
      getAllUserAddressesBalancesSinceTimePoint(newerThan);

  for (auto& b : userAddressBalances) {
    hub::rpc::BalanceEvent event;
    hub::rpc::UserAddressBalanceEvent* userAddressEvent =
        new hub::rpc::UserAddressBalanceEvent();
    userAddressEvent->set_userid(std::move(b.userIdentifier));
    userAddressEvent->set_timestamp(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            b.timestamp.time_since_epoch())
            .count());
    userAddressEvent->set_amount(b.amount);
    userAddressEvent->set_reason(userAddressBalanceEventTypeFromSql(b.reason));
    userAddressEvent->set_hash(std::move(b.hash));
    userAddressEvent->set_useraddress(std::move(b.userAddress));
    userAddressEvent->set_message(std::move(b.message));

    event.set_allocated_useraddressevent(userAddressEvent);
    if (!writer->Write(event)) {
      return grpc::Status::CANCELLED;
    }
  }

  auto hubAddressBalances = getAllHubAddressesBalancesSinceTimePoint(newerThan);

  for (auto& b : hubAddressBalances) {
    hub::rpc::BalanceEvent event;
    hub::rpc::HubAddressBalanceEvent* hubAddressEvent =
        new hub::rpc::HubAddressBalanceEvent();
    hubAddressEvent->set_hubaddress(std::move(b.hubAddress));
    hubAddressEvent->set_timestamp(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            b.timestamp.time_since_epoch())
            .count());
    hubAddressEvent->set_amount(b.amount);
    hubAddressEvent->set_sweepbundlehash(std::move(b.sweepBundleHash));
    hubAddressEvent->set_reason(hubAddressBalanceTypeFromSql(b.reason));
    event.set_allocated_hubaddressevent(hubAddressEvent);
    if (!writer->Write(event)) {
      return grpc::Status::CANCELLED;
    }
  }

  return grpc::Status::OK;
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
