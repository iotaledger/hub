// Copyright 2018 IOTA Foundation

#include <iostream>
#include <thread>

#include "common/converter.h"
#include "hub/commands/converter.h"
#include "hub/commands/factory.h"
#include "hub/commands/helper.h"
#include "hub/db/helper.h"

#include "hub/commands/sweep_subscription.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<SweepSubscription> registrator;

boost::property_tree::ptree SweepSubscription::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  SweepSubscriptionRequest req;
  std::vector<SweepEvent> rep;

  auto maybeNewerThan = request.get_optional<std::string>("newerThan");
  if (maybeNewerThan) {
    std::istringstream iss(maybeNewerThan.value());
    iss >> req.newerThan;
  }

  auto status = doProcess(&req, &rep);

  if (status != common::cmd::OK) {
    tree.add("error", common::cmd::getErrorString(status));
  } else {
    int i = 0;
    for (auto event : rep) {
      auto eventId = "event_" + std::to_string(i++);
      tree.add(eventId, "");
      tree.put(eventId + ".bundleHash", event.bundleHash);
      tree.put(eventId + ".timestamp", event.timestamp);

      for (auto uuid : event.uuids) {
        tree.add(eventId + ".withdrawalUuid", uuid);
      }
    }
  }

  return tree;
}

common::cmd::Error SweepSubscription::doProcess(
    const SweepSubscriptionRequest* request,
    std::vector<SweepEvent>* events) noexcept {
  std::chrono::milliseconds dur(request->newerThan);
  std::chrono::time_point<std::chrono::system_clock> newerThan(dur);

  auto sweeps = getSweeps(newerThan);
  for (auto& s : sweeps) {
    SweepEvent event;
    event.bundleHash = std::move(s.bundleHash);
    event.timestamp = common::timepointToUint64(s.timestamp);
    auto& uuids = s.withdrawalUUIDs;

    std::for_each(uuids.begin(), uuids.end(), [&](std::string& uuid) {
      event.uuids.emplace_back(std::move(uuid));
    });

    events->emplace_back(std::move(event));
  }
  return common::cmd::OK;
}

std::vector<db::SweepEvent> SweepSubscription::getSweeps(
    std::chrono::system_clock::time_point newerThan) {
  return db::DBManager::get().connection().getSweeps(newerThan);
}

}  // namespace cmd
}  // namespace hub
