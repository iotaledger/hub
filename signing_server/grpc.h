/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_CRYPTO_GRPC_H_
#define HUB_CRYPTO_GRPC_H_

#include <memory>

#include <grpc++/grpc++.h>

#include "proto/signing_server.grpc.pb.h"
#include "proto/signing_server.pb.h"
#include "proto/signing_server_messages.pb.h"

namespace hub {
namespace rpc {
namespace crypto {

class SigningServerImpl final : public crypto::SigningServer::Service {
 public:
  /// Constructor
  SigningServerImpl() {}
  /// Destructor
  ~SigningServerImpl() override {}

  // Gets the address for the UUID
  grpc::Status GetAddressForUUID(
      ::grpc::ServerContext* context,
      const crypto::GetAddressForUUIDRequest* request,
      crypto::GetAddressForUUIDReply* response) override;
  // Gets the signature for the UUID
  grpc::Status GetSignatureForUUID(
      ::grpc::ServerContext* context,
      const crypto::GetSignatureForUUIDRequest* request,
      crypto::GetSignatureForUUIDReply* response) override;
  // Gets the security level of the provider
  grpc::Status GetSecurityLevel(
      ::grpc::ServerContext* context,
      const crypto::GetSecurityLevelRequest* request,
      crypto::GetSecurityLevelReply* response) override;
};

}  // namespace crypto
}  // namespace rpc
}  // namespace hub

#endif  // HUB_CRYPTO_GRPC_H_