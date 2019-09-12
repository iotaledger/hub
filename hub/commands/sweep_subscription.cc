// Copyright 2018 IOTA Foundation

#include "hub/commands/sweep_subscription.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include <utility>

#include "common/stats/session.h"
#include "hub/commands/factory.h"
#include "hub/commands/helper.h"
#include "hub/commands/proto_sql_converter.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "proto/hub.grpc.pb.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<SweepSubscription> registrator;

boost::property_tree::ptree SweepSubscription::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  return tree;
}

grpc::Status SweepSubscription::doProcess(
    const hub::rpc::SweepSubscriptionRequest* request,
    grpc::ServerWriterInterface<hub::rpc::SweepEvent>* writer) noexcept {
  std::chrono::milliseconds dur(request->newerthan());
  std::chrono::time_point<std::chrono::system_clock> newerThan(dur);

  auto sweeps = getSweeps(newerThan);
  bool cancelled = false;
  for (auto& s : sweeps) {
    hub::rpc::SweepEvent event;
    event.set_bundlehash(std::move(s.bundleHash));
    event.set_timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
                            s.timestamp.time_since_epoch())
                            .count());
    auto& uuids = s.withdrawalUUIDs;

    std::for_each(uuids.begin(), uuids.end(), [&](std::string& uuid) {
      event.add_withdrawaluuid(std::move(uuid));
    });

    if (!writer->Write(std::move(event))) {
      cancelled = true;
      break;
    }
  }
  if (cancelled) {
    return grpc::Status::CANCELLED;
  }
  return grpc::Status::OK;
}

std::vector<db::SweepEvent> SweepSubscription::getSweeps(
    std::chrono::system_clock::time_point newerThan) {
  return db::DBManager::get().connection().getSweeps(newerThan);
}

}  // namespace cmd
}  // namespace hub
