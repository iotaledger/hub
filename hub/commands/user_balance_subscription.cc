// Copyright 2018 IOTA Foundation

#include "hub/commands/user_balance_subscription.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include <utility>

#include "hub/commands/helper.h"
#include "hub/commands/proto_sql_converter.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/stats/session.h"
#include "proto/hub.grpc.pb.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

namespace hub {
namespace cmd {

grpc::Status UserBalanceSubscription::doProcess(
    const hub::rpc::UserBalanceSubscriptionRequest* request,
    grpc::ServerWriterInterface<hub::rpc::UserBalanceEvent>* writer) noexcept {
  std::chrono::milliseconds dur(request->newerthan());
  std::chrono::time_point<std::chrono::system_clock> newerThan(dur);

  hub::rpc::UserBalanceEvent event;
  auto balances = getAccountBalances(newerThan);
  bool cancelled = false;
  for (auto& b : balances) {
    event.set_userid(std::move(b.userIdentifier));
    event.set_timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
                            b.timestamp.time_since_epoch())
                            .count());
    event.set_amount(b.amount);
    event.set_type(userBalanceEventTypeFromSql(b.type));
    if (!writer->Write(event)) {
      cancelled = true;
      break;
    }
  }
  if (cancelled) {
    return grpc::Status::CANCELLED;
  }
  return grpc::Status::OK;
}

std::vector<db::UserBalanceEvent> UserBalanceSubscription::getAccountBalances(
    std::chrono::system_clock::time_point newerThan) {
  return db::DBManager::get().connection().getAccountBalances(newerThan);
}

}  // namespace cmd
}  // namespace hub
