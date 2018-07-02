/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/sweep_info.h"

#include <cstdint>
#include <nonstd/optional.hpp>
#include <utility>

#include "common/stats/session.h"
#include "common/types/types.h"
#include "hub/commands/helper.h"
#include "hub/db/connection.h"
#include "hub/db/types.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

namespace hub {
namespace cmd {

grpc::Status SweepInfo::doProcess(const hub::rpc::SweepInfoRequest* request,
                                  hub::rpc::SweepEvent* response) noexcept {
  auto& connection = db::DBManager::get().connection();

  nonstd::optional<hub::db::SweepEvent> maybeEvent;

  if (request->requestBy_case() ==
      hub::rpc::SweepInfoRequest::kWithdrawalUUID) {
    try {
      // We could parse the withdrawal UUID to boost here for validation.
      // But we can just as well ask the DB.
      maybeEvent =
          connection.getSweepByWithdrawalUUID(request->withdrawaluuid());
    } catch (const std::exception& ex) {
      // ignore, DB !probably failed.
    }
  } else if (request->requestBy_case() ==
             hub::rpc::SweepInfoRequest::kBundleHash) {
    try {
      common::crypto::Hash bundleHash(request->bundlehash());
      maybeEvent = connection.getSweepByBundleHash(bundleHash);
    } catch (const std::exception& ex) {
      // ignore, hash parsing probably failed.
    }
  }

  if (!maybeEvent) {
    return grpc::Status::CANCELLED;
  }

  response->set_bundlehash(std::move(maybeEvent->bundleHash));
  response->set_timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
                              maybeEvent->timestamp.time_since_epoch())
                              .count());
  auto& uuids = maybeEvent->withdrawalUUIDs;
  std::for_each(uuids.begin(), uuids.end(), [&](std::string& uuid) {
    response->add_withdrawaluuid(std::move(uuid));
  });

  return grpc::Status::OK;
}

}  // namespace cmd
}  // namespace hub
