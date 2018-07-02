/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_CRYPTO_PROVIDER_GET_SIGNATURE_FOR_UUID_H_
#define HUB_COMMANDS_CRYPTO_PROVIDER_GET_SIGNATURE_FOR_UUID_H_

#include <string>

#include "hub/commands/command.h"

namespace hub {
namespace rpc {
namespace crypto {
class GetSignatureForUUIDRequest;
class GetSignatureForUUIDReply;
}  // namespace crypto
}  // namespace rpc

namespace cmd {

/// Gets information on an signature
/// @param[in] hub::rpc::GetSignatureForUUIDRequest
/// @param[in] hub::rpc::GetSignatureForUUIDReply
class GetSignatureForUUID
    : public Command<rpc::crypto::GetSignatureForUUIDRequest,
                     rpc::crypto::GetSignatureForUUIDReply> {
 public:
  using Command<rpc::crypto::GetSignatureForUUIDRequest,
                rpc::crypto::GetSignatureForUUIDReply>::Command;

  const std::string name() override { return "GetSignatureForUUID"; }

  grpc::Status doProcess(
      const rpc::crypto::GetSignatureForUUIDRequest* request,
      rpc::crypto::GetSignatureForUUIDReply* response) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_CRYPTO_PROVIDER_GET_SIGNATURE_FOR_UUID_H_
