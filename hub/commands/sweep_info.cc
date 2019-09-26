/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include <cstdint>

#include <nonstd/optional.hpp>

#include "common/converter.h"
#include "common/crypto/types.h"
#include "hub/commands/factory.h"
#include "hub/commands/helper.h"
#include "hub/db/helper.h"

#include "hub/commands/sweep_info.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<SweepInfo> registrator;

boost::property_tree::ptree SweepInfo::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  SweepInfoRequest req;
  SweepEvent rep;

  auto maybeRequestByUuid = request.get_optional<std::string>("requestByUuid");
  if (maybeRequestByUuid) {
    req.requestByUuid = common::stringToBool(maybeRequestByUuid.value());

    if (req.requestByUuid) {
      auto maybeWithdrawalUuid =
          request.get_optional<std::string>("withdrawalUuid");
      if (!maybeWithdrawalUuid) {
        tree.add("error",
                 common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
        return tree;
      } else {
        req.uuid = maybeWithdrawalUuid.value();
      }
    } else {
      auto maybeBundleHash = request.get_optional<std::string>("bundleHash");
      if (!maybeBundleHash) {
        tree.add("error",
                 common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
        return tree;
      } else {
        req.bundleHash = maybeBundleHash.value();
      }
    }

    auto status = doProcess(&req, &rep);

    if (status != common::cmd::OK) {
      tree.add("error", common::cmd::getErrorString(status));
    } else {
      tree.add("bundleHash", rep.bundleHash);
      tree.add("timestamp", rep.timestamp);

      for (auto uuid : rep.uuids) {
        if (!uuid.empty()) {
          tree.add("withdrawalUuid", uuid);
        }
      }
    }
  } else {
    tree.add("error",
             common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
  }

  return tree;
}

common::cmd::Error SweepInfo::doProcess(const SweepInfoRequest* request,
                                        SweepEvent* response) noexcept {
  auto& connection = db::DBManager::get().connection();

  nonstd::optional<hub::db::SweepEvent> maybeEvent;

  if (request->requestByUuid) {
    try {
      // We could parse the withdrawal UUID to boost here for validation.
      // But we can just as well ask the DB.
      maybeEvent = connection.getSweepByWithdrawalUUID(request->uuid);
    } catch (const std::exception& ex) {
      // ignore, DB !probably failed.
    }
  } else {
    try {
      common::crypto::Hash bundleHash(request->bundleHash);
      maybeEvent = connection.getSweepByBundleHash(bundleHash);
    } catch (const std::exception& ex) {
      // ignore, hash parsing probably failed.
    }
  }

  if (!maybeEvent) {
    return common::cmd::CANCELLED;
  }

  response->bundleHash = std::move(maybeEvent->bundleHash);
  response->timestamp = common::timepointToUint64(maybeEvent->timestamp);
  auto& uuids = maybeEvent->withdrawalUUIDs;
  std::for_each(uuids.begin(), uuids.end(), [&](std::string& uuid) {
    response->uuids.emplace_back(std::move(uuid));
  });

  return common::cmd::OK;
}

}  // namespace cmd
}  // namespace hub
