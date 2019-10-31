/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing Secretrmation
 */

#include <cstdint>
#include <utility>

#include "common/converter.h"
#include "common/crypto/manager.h"
#include "common/crypto/types.h"
#include "hub/auth/manager.h"
#include "hub/bundle/bundle_utils.h"
#include "hub/commands/factory.h"
#include "hub/commands/helper.h"
#include "hub/db/helper.h"

#include "hub/commands/recover_funds.h"

namespace hub {
namespace cmd {

DEFINE_bool(RecoverFunds_enabled, false,
            "Whether the RecoverFunds API call should be available");

static CommandFactoryRegistrator<RecoverFunds> registrator;

boost::property_tree::ptree RecoverFunds::doProcess(
    const boost::property_tree::ptree& request) noexcept {
  boost::property_tree::ptree tree;

  if (!FLAGS_RecoverFunds_enabled) {
    LOG(ERROR) << session() << ": Recover funds is disabled";
    tree.add("error", common::cmd::getErrorString(common::cmd::CANCELLED));
    return tree;
  }

  RecoverFundsRequest req;
  RecoverFundsReply rep;

  auto maybeUserId = request.get_optional<std::string>("userId");
  if (!maybeUserId) {
    tree.add("error",
             common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
    return tree;
  }

  req.userId = maybeUserId.value();

  auto maybeAddress = request.get_optional<std::string>("address");
  if (!maybeAddress) {
    tree.add("error",
             common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
    return tree;
  }

  req.address = maybeAddress.value();

  auto maybePayoutAddress = request.get_optional<std::string>("payoutAddress");
  if (!maybePayoutAddress) {
    tree.add("error",
             common::cmd::getErrorString(common::cmd::MISSING_ARGUMENT));
    return tree;
  }
  req.payoutAddress = maybePayoutAddress.value();
  req.validateChecksum = false;

  auto maybeValidateChecksum =
      request.get_optional<std::string>("validateChecksum");
  if (maybeValidateChecksum) {
    req.validateChecksum = common::stringToBool(maybeValidateChecksum.value());
  }

  auto status = doProcess(&req, &rep);

  if (status != common::cmd::OK) {
    tree.add("error", common::cmd::getErrorString(status));
  }

  return tree;
}

common::cmd::Error RecoverFunds::doProcess(
    const RecoverFundsRequest* request, RecoverFundsReply* response) noexcept {
  if (!FLAGS_RecoverFunds_enabled) {
    LOG(ERROR) << session() << ": Recover funds is disabled";
    return common::cmd::CANCELLED;
  }

  auto& connection = db::DBManager::get().connection();
  try {
    uint64_t userId;

    // Get userId for identifier
    {
      auto maybeUserId = connection.userIdFromIdentifier(request->userId);
      if (!maybeUserId) {
        return common::cmd::USER_DOES_NOT_EXIST;
      }

      userId = maybeUserId.value();
    }

    nonstd::optional<common::crypto::Address> address = {
        common::crypto::Address(request->address)};
    nonstd::optional<common::crypto::Address> payoutAddress;

    if (request->validateChecksum) {
      payoutAddress =
          std::move(common::crypto::CryptoManager::get()
                        .provider()
                        .verifyAndStripChecksum(request->payoutAddress));

      if (!payoutAddress.has_value()) {
        return common::cmd::INVALID_CHECKSUM;
      }
    } else {
      payoutAddress = {common::crypto::Address(request->payoutAddress)};
    }

    // 1. Check that address was used before
    auto maybeAddressInfo = connection.getAddressInfo(address.value());
    if (!maybeAddressInfo) {
      return common::cmd::UNKNOWN_ADDRESS;
    }

    if (maybeAddressInfo->userId.compare(request->userId) != 0) {
      return common::cmd::WRONG_USER_ADDRESS;
    }

    if (!maybeAddressInfo->usedForSweep) {
      return common::cmd::INVALID_ADDRESS;
    }

    // Verify payout address wasn't spent before
    auto res = _api->wereAddressesSpentFrom({payoutAddress.value().str()});
    if (!res.has_value() || res.value().states.empty()) {
      return common::cmd::IOTA_NODE_UNAVAILABLE;
    } else if (res.value().states.front()) {
      return common::cmd::ADDRESS_WAS_SPENT;
    }

    const auto& iriBalances = _api->getBalances({request->address});
    if (!iriBalances) {
      return common::cmd::ADDRESS_NOT_KNOWN_TO_NODE;
    }

    auto amount = iriBalances.value().at(request->address);

    if (amount == 0) {
      return common::cmd::ADDRESS_BALANCE_ZERO;
    }

    std::vector<hub::db::TransferInput> deposits;

    hub::db::TransferInput ti = {
      addressId : static_cast<int64_t>(maybeAddressInfo.value().id),
      userId : userId,
      address : address.value(),
      uuid : maybeAddressInfo.value().uuid,
      amount : amount
    };
    deposits.push_back(std::move(ti));

    std::vector<hub::db::TransferOutput> outputs;

    // Create the "withdrawl"
    outputs.emplace_back(hub::db::TransferOutput{
      id : -1,
      amount : amount,
      tag : {},
      payoutAddress : payoutAddress.value()
    });

    std::vector<std::string> alreadySignedBundleHashes;
    std::unordered_multimap<std::string, cppclient::Bundle>
        alreadySignedBundles = _api->getConfirmedBundlesForAddresses(
            {address.value().str()}, false);

    for (auto const& [key, bundle] : alreadySignedBundles) {
      for (auto const& tx : bundle) {
        if (tx.value < 0) {
          alreadySignedBundleHashes.emplace_back(tx.bundleHash);
        }
      }
    }

    auto bundle = hub::bundle_utils::createBundle(deposits, {}, outputs, {},
                                                  alreadySignedBundleHashes);

  } catch (const std::exception& ex) {
    return common::cmd::UNKNOWN_ERROR;
  }

  return common::cmd::OK;
}

}  // namespace cmd
}  // namespace hub
