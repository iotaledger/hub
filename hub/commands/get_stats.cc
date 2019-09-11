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

std::string GetStats::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  return "Not implemented\n";
}

grpc::Status GetStats::doProcess(const hub::rpc::GetStatsRequest* request,
                                 hub::rpc::GetStatsReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();

  response->set_totalbalance(connection.getTotalBalance());

  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace hub
