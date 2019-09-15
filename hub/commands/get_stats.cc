/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/get_stats.h"

#include <cstdint>

#include <sqlpp11/connection.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

#include "common/stats/session.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/factory.h"
#include "hub/commands/helper.h"

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
    tree.add("error", common::cmd::errorToStringMap.at(status));
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
