/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include <cstdint>

#include "hub/commands/factory.h"
#include "hub/commands/helper.h"
#include "hub/db/helper.h"

#include "hub/commands/get_stats.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<GetStats> registrator;

boost::property_tree::ptree GetStats::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;
  GetStatsRequest req;
  GetStatsReply rep;
  auto status = doProcess(&req, &rep);

  if (status != common::cmd::OK) {
    tree.add("error", common::cmd::getErrorString(status));
  } else {
    tree.add("totalBalance", rep.totalBalance);
  }
  return tree;
}

common::cmd::Error GetStats::doProcess(const GetStatsRequest* request,
                                       GetStatsReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();

  response->totalBalance = connection.getTotalBalance();

  return common::cmd::OK;
}

}  // namespace cmd

}  // namespace hub
