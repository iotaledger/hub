/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_CRYPTO_PROVIDER_GET_SECURITY_LEVEL_H_
#define HUB_COMMANDS_CRYPTO_PROVIDER_GET_SECURITY_LEVEL_H_

#include <string>

#include "hub/commands/command.h"

namespace hub {
namespace rpc {
namespace crypto {
class GetSecurityLevelRequest;
class GetSecurityLevelReply;
}  // namespace crypto
}  // namespace rpc

namespace cmd {

/// Gets information on an signature
/// @param[in] hub::rpc::GetSignatureForUUIDRequest
/// @param[in] hub::rpc::GetSignatureForUUIDReply
class GetSecurityLevel : public Command<rpc::crypto::GetSecurityLevelRequest,
                                        rpc::crypto::GetSecurityLevelReply> {
 public:
  using Command<rpc::crypto::GetSecurityLevelRequest,
                rpc::crypto::GetSecurityLevelReply>::Command;

  const std::string name() override { return "GetSignatureForUUID"; }

  grpc::Status doProcess(
      const rpc::crypto::GetSecurityLevelRequest* request,
      rpc::crypto::GetSecurityLevelReply* response) noexcept override;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_CRYPTO_PROVIDER_GET_SECURITY_LEVEL_H_
