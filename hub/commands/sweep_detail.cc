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

#include "hub/commands/sweep_detail.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<SweepDetail> registrator;

boost::property_tree::ptree SweepDetail::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  SweepDetailRequest req;
  SweepDetailReply rep;

  auto maybeBundleHash = request.get_optional<std::string>("bundleHash");
  if (maybeBundleHash) {
    req.bundleHash = maybeBundleHash.value();

    auto status = doProcess(&req, &rep);

    if (status != common::cmd::OK) {
      tree.add("error", common::cmd::getErrorString(status));
    } else {
      tree.add("confirmed", common::boolToString(rep.confirmed));

      for (auto txTrytes : rep.trytes) {
        tree.add("trytes", txTrytes);
      }

      for (auto tailHash : rep.tailHashes) {
        tree.add("tailHash", tailHash);
      }
    }
  } else {
    tree.add("error",
             common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
  }

  return tree;
}

common::cmd::Error SweepDetail::doProcess(const SweepDetailRequest* request,
                                          SweepDetailReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();

  nonstd::optional<hub::db::SweepDetail> maybeDetail;

  try {
    common::crypto::Hash bundleHash(request->bundleHash);
    maybeDetail = connection.getSweepDetailByBundleHash(bundleHash);
  } catch (const std::exception& ex) {
    // ignore, hash parsing probably failed.
  }

  if (!maybeDetail) {
    return common::cmd::CANCELLED;
  }

  response->confirmed = maybeDetail->confirmed;
  auto trytesVec = std::move(maybeDetail->trytes);
  std::for_each(trytesVec.begin(), trytesVec.end(), [&](std::string& trytes) {
    response->trytes.emplace_back(std::move(trytes));
  });

  auto tailsVec = std::move(maybeDetail->tails);
  std::for_each(tailsVec.begin(), tailsVec.end(),
                [&](common::crypto::Hash& tail) {
                  response->tailHashes.emplace_back(std::move(tail.str()));
                });

  return common::cmd::OK;
}

}  // namespace cmd
}  // namespace hub
