// Copyright 2018 IOTA Foundation

#include "hub/commands/sweep_subscription.h"

#include <chrono>
#include <functional>
#include <iostream>
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

static CommandFactoryRegistrator<SweepSubscription> registrator;

boost::property_tree::ptree SweepSubscription::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  SweepSubscriptionRequest req;
  std::vector<SweepEvent> rep;

  auto maybeAmount = request.get_optional<std::string>("newerThan");
  if (maybeAmount) {
    std::istringstream iss(maybeAmount.value());
    iss >> req.newerThan;
  }

  auto status = doProcess(&req, &rep);

  if (status != common::cmd::OK) {
    tree.add("error", common::cmd::errorToStringMap.at(status));
  } else {
    int i = 0;
    for (auto event : rep) {
      auto eventId = "event_" + std::to_string(i++);
      tree.add(eventId, "");
      tree.put(eventId + ".bundleHash", event.bundleHash);
      tree.put(eventId + ".timestamp", event.timestamp);

      auto uuidNum = 0;
      for (auto uuid : event.uuids) {
        auto uuidId = "uuid_" + std::to_string(uuidNum++);
        tree.put(eventId + "." + uuidId, uuid);
      }
    }
  }

  return tree;
}

common::cmd::Error SweepSubscription::doProcess(
    const SweepSubscriptionRequest* request,
    std::vector<SweepEvent>* writer) noexcept {
  std::chrono::milliseconds dur(request->newerThan);
  std::chrono::time_point<std::chrono::system_clock> newerThan(dur);

  auto sweeps = getSweeps(newerThan);
  for (auto& s : sweeps) {
    SweepEvent event;
    event.bundleHash = std::move(s.bundleHash);
    event.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                          s.timestamp.time_since_epoch())
                          .count();
    auto& uuids = s.withdrawalUUIDs;

    std::for_each(uuids.begin(), uuids.end(), [&](std::string& uuid) {
      event.uuids.emplace_back(std::move(uuid));
    });

    writer->emplace_back(std::move(event));
  }
  return common::cmd::OK;
}

std::vector<db::SweepEvent> SweepSubscription::getSweeps(
    std::chrono::system_clock::time_point newerThan) {
  return db::DBManager::get().connection().getSweeps(newerThan);
}

}  // namespace cmd
}  // namespace hub
