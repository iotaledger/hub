/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/sweep_detail.h"

#include <cstdint>
#include <nonstd/optional.hpp>
#include <utility>

#include "common/crypto/types.h"
#include "common/stats/session.h"
#include "hub/commands/factory.h"
#include "hub/commands/helper.h"
#include "hub/db/connection.h"
#include "hub/db/types.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<SweepDetail> registrator;

boost::property_tree::ptree SweepDetail::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  return tree;
}

grpc::Status SweepDetail::doProcess(
    const hub::rpc::SweepDetailRequest* request,
    hub::rpc::SweepDetailReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();

  nonstd::optional<hub::db::SweepDetail> maybeDetail;

  try {
    common::crypto::Hash bundleHash(request->bundlehash());
    maybeDetail = connection.getSweepDetailByBundleHash(bundleHash);
  } catch (const std::exception& ex) {
    // ignore, hash parsing probably failed.
  }

  if (!maybeDetail) {
    return grpc::Status::CANCELLED;
  }

  response->set_confirmed(maybeDetail->confirmed);
  auto trytesVec = std::move(maybeDetail->trytes);
  std::for_each(trytesVec.begin(), trytesVec.end(), [&](std::string& trytes) {
    response->add_trytes(std::move(trytes));
  });

  auto tailsVec = std::move(maybeDetail->tails);
  std::for_each(tailsVec.begin(), tailsVec.end(),
                [&](common::crypto::Hash& tail) {
                  response->add_tailhash(std::move(tail.str()));
                });

  return grpc::Status::OK;
}

}  // namespace cmd
}  // namespace hub
