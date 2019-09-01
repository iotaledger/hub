/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing Secretrmation
 */

#include "hub/commands/recover_funds.h"

#include <cstdint>
#include <utility>

#include <sqlpp11/exception.h>

#include "common/crypto/manager.h"
#include "common/crypto/types.h"
#include "common/stats/session.h"
#include "hub/auth/manager.h"
#include "hub/bundle/bundle_utils.h"
#include "hub/commands/helper.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/db/types.h"
#include "proto/hub.pb.h"
#include "schema/schema.h"

namespace hub {
namespace cmd {

grpc::Status RecoverFunds::doProcess(
    const hub::rpc::RecoverFundsRequest* request,
    hub::rpc::RecoverFundsReply* response) noexcept {
  auto& connection = db::DBManager::get().connection();
  auto& cryptoProvider = common::crypto::CryptoManager::get().provider();
  auto& authProvider = auth::AuthManager::get().provider();

  try {
    uint64_t userId;

    // Get userId for identifier
    {
      auto maybeUserId = connection.userIdFromIdentifier(request->userid());
      if (!maybeUserId) {
        return grpc::Status(
            grpc::StatusCode::FAILED_PRECONDITION, "",
            errorToString(hub::rpc::ErrorCode::USER_DOES_NOT_EXIST));
      }

      userId = maybeUserId.value();
    }

    nonstd::optional<common::crypto::Address> address = {
        common::crypto::Address(request->address())};
    nonstd::optional<common::crypto::Address> payoutAddress;

    if (request->validatechecksum()) {
      payoutAddress =
          std::move(common::crypto::CryptoManager::get()
                        .provider()
                        .verifyAndStripChecksum(request->payoutaddress()));

      if (!payoutAddress.has_value()) {
        return grpc::Status(
            grpc::StatusCode::FAILED_PRECONDITION, "",
            errorToString(hub::rpc::ErrorCode::CHECKSUM_INVALID));
      }
    } else {
      payoutAddress = {common::crypto::Address(request->payoutaddress())};
    }

    // 1. Check that address was used before
    auto maybeAddressInfo = connection.getAddressInfo(address.value());
    if (!maybeAddressInfo) {
      return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                          errorToString(hub::rpc::ErrorCode::UNKNOWN_ADDRESS));
    }

    if (maybeAddressInfo->userId.compare(request->userid()) != 0) {
      return grpc::Status(
          grpc::StatusCode::FAILED_PRECONDITION, "",
          errorToString(hub::rpc::ErrorCode::WRONG_USER_ADDRESS));
    }

    if (!maybeAddressInfo->usedForSweep) {
      return grpc::Status(
          grpc::StatusCode::FAILED_PRECONDITION, "",
          errorToString(hub::rpc::ErrorCode::INELIGIBLE_ADDRESS));
    }

    // Verify payout address wasn't spent before
    if (_api) {
      auto res = _api->wereAddressesSpentFrom({payoutAddress.value().str()});
      if (!res.has_value() || res.value().states.empty()) {
        return grpc::Status(
            grpc::StatusCode::FAILED_PRECONDITION, "",
            errorToString(hub::rpc::ErrorCode::IRI_CLIENT_UNAVAILABLE));
      } else if (res.value().states.front()) {
        return grpc::Status(
            grpc::StatusCode::FAILED_PRECONDITION, "",
            errorToString(hub::rpc::ErrorCode::ADDRESS_WAS_ALREADY_SPENT));
      }
    }

    const auto& iriBalances = _api->getBalances({request->address()});
    if (!iriBalances) {
      return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                          errorToString(hub::rpc::ErrorCode::UNKNOWN_ADDRESS));
    }

    auto amount = iriBalances.value().at(request->address());

    if (amount == 0) {
      return grpc::Status(
          grpc::StatusCode::FAILED_PRECONDITION, "",
          errorToString(hub::rpc::ErrorCode::ADDRESS_BALANCE_ZERO));
    }

    std::vector<hub::db::TransferInput> deposits;

    hub::db::TransferInput ti = {maybeAddressInfo.value().id, userId,
                                 address.value(), maybeAddressInfo.value().uuid,
                                 -amount};
    deposits.push_back(std::move(ti));

    auto hubOutput = hub::bundle_utils::getHubOutput(0);

    std::vector<hub::db::TransferOutput> outputs;

    // Create the "withdrawl"
    outputs.emplace_back(
        hub::db::TransferOutput{-1, amount, {}, payoutAddress.value()});

    auto bundle =
        hub::bundle_utils::createBundle(deposits, {}, outputs, hubOutput);

    connection.createSweep(std::get<0>(bundle), std::get<1>(bundle),
                           hubOutput.id);

  } catch (const std::exception& ex) {
    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(hub::rpc::ErrorCode::EC_UNKNOWN));
  }

  return grpc::Status::OK;
}

}  // namespace cmd
}  // namespace hub
