/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_CRYPTO_PROVIDER_GET_ADDRESS_FOR_UUID_H_
#define HUB_COMMANDS_CRYPTO_PROVIDER_GET_ADDRESS_FOR_UUID_H_

#include <string>

#include "hub/commands/command.h"

namespace hub {
namespace rpc {
namespace crypto {
class GetAddressForUUIDRequest;
class GetAddressForUUIDReply;
}  // namespace crypto
}  // namespace rpc

namespace cmd {

/// Gets information on an address
/// @param[in] hub::rpc::GetAddressForUUIDRequest
/// @param[in] hub::rpc::GetAddressForUUIDReply
class GetAddressForUUID : public Command<rpc::crypto::GetAddressForUUIDRequest,
                                         rpc::crypto::GetAddressForUUIDReply> {
 public:
  using Command<rpc::crypto::GetAddressForUUIDRequest,
                rpc::crypto::GetAddressForUUIDReply>::Command;

  const std::string name() override { return "GetAddressForUUID"; }

  grpc::Status doProcess(
      const rpc::crypto::GetAddressForUUIDRequest* request,
      rpc::crypto::GetAddressForUUIDReply* response) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_CRYPTO_PROVIDER_GET_ADDRESS_FOR_UUID_H_
