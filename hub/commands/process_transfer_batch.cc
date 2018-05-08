// Copyright 2018 IOTA Foundation

#include "hub/commands/process_transfer_batch.h"

#include <cstdint>
#include <vector>
#include <set>

#include <sqlpp11/connection.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/stats/session.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/commands/helper.h"
#include "hub/commands/proto_sql_converter.h"

namespace hub {
namespace cmd {

grpc::Status ProcessTransferBatch::doProcess(
    const hub::rpc::ProcessTransferBatchRequest* request,
    hub::rpc::ProcessTransferBatchReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();
  std::set<std::string> identifiers;
  bool zeroAmount = false;
  {
    //populate identifiers for db query and validate amounts are not zero
    for (auto i = 0; i < request->transfers_size(); ++i) {
      const hub::rpc::ProcessTransferBatchRequest_Transfer& t =
          request->transfers(i);
      identifiers.insert(t.userid());
      if (t.amount() == 0) {
        zeroAmount = true;
      }
    }
  }

  if (zeroAmount) {
    return grpc::Status(
        grpc::StatusCode::FAILED_PRECONDITION, "",
        errorToString(hub::rpc::ErrorCode::ZERO_AMOUNT_TRANSFER));
  }

  auto identifierToId = db::userIdsFromIdentifiers(connection, identifiers);

  if (identifierToId.size() < identifiers.size()) {
    return grpc::Status(
        grpc::StatusCode::FAILED_PRECONDITION, "",
        errorToString(hub::rpc::ErrorCode::USER_DOES_NOT_EXIST));
  }

  //Actual transfer insertion to db
  std::vector<hub::db::UserTransfer> transfers;
  for (auto i = 0; i < request->transfers_size(); ++i) {
    const hub::rpc::ProcessTransferBatchRequest_Transfer& t =
        request->transfers(i);
    transfers.emplace_back(
        hub::db::UserTransfer{identifierToId[t.userid()], t.amount()});
  }
  insertUserTransfers(connection, transfers);

  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace hub
