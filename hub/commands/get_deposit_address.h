/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_GET_DEPOSIT_ADDRESS_H_
#define HUB_COMMANDS_GET_DEPOSIT_ADDRESS_H_

#include <string>

#include "common/command.h"

namespace hub {
namespace rpc {
class GetDepositAddressRequest;
class GetDepositAddressReply;
}  // namespace rpc

namespace cmd {

/// Gets a new deposit address.
/// @param[in] hub::rpc::GetDepositAddressRequest
/// @param[in] hub::rpc::GetDepositAddressReply
class GetDepositAddress
    : public common::cmd::Command<hub::rpc::GetDepositAddressRequest,
                                  hub::rpc::GetDepositAddressReply> {
 public:
  using Command<hub::rpc::GetDepositAddressRequest,
                hub::rpc::GetDepositAddressReply>::Command;

  const std::string name() override { return "GetDepositAddress"; }

  grpc::Status doProcess(
      const hub::rpc::GetDepositAddressRequest* request,
      hub::rpc::GetDepositAddressReply* response) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_GET_DEPOSIT_ADDRESS_H_
