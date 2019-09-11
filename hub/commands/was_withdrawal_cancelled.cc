/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/commands/was_withdrawal_cancelled.h"

#include <cstdint>

#include <sqlpp11/connection.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "common/stats/session.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/factory.h"
#include "hub/commands/helper.h"

namespace hub {
namespace cmd {

static CommandFactoryRegistrator<WasWithdrawalCancelled> registrator;

std::string WasWithdrawalCancelled::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  return "Not implemented\n";
}

grpc::Status WasWithdrawalCancelled::doProcess(
    const hub::rpc::WasWithdrawalCancelledRequest* request,
    hub::rpc::WasWithdrawalCancelledReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();

  try {
    boost::uuids::uuid uuid = boost::uuids::string_generator()(request->uuid());

    auto withdrawalInfo =
        connection.getWithdrawalInfoFromUUID(boost::uuids::to_string(uuid));

    response->set_wascancelled(withdrawalInfo.wasCancelled);

  } catch (const std::exception& ex) {
    LOG(ERROR) << session() << " Query failed: " << ex.what();
    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(hub::rpc::ErrorCode::INVALID_UUID));
  }

  return grpc::Status::OK;
}  // namespace cmd

}  // namespace cmd
}  // namespace hub
