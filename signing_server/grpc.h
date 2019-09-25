/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef SIGNING_SERVER_GRPC_H_
#define SIGNING_SERVER_GRPC_H_

#include <memory>

#include <grpc++/grpc++.h>

#include "proto/signing_server.grpc.pb.h"
#include "proto/signing_server.pb.h"
#include "proto/signing_server_messages.pb.h"

namespace signing {
namespace rpc {

class SigningServerImpl final : public SigningServer::Service {
 public:
  /// Constructor
  SigningServerImpl() {}
  /// Destructor
  ~SigningServerImpl() override {}

  // Gets the address for the UUID
  grpc::Status GetAddressForUUID(::grpc::ServerContext* context,
                                 const GetAddressForUUIDRequest* request,
                                 GetAddressForUUIDReply* response) override;
  // Gets the signature for the UUID
  grpc::Status GetSignatureForUUID(::grpc::ServerContext* context,
                                   const GetSignatureForUUIDRequest* request,
                                   GetSignatureForUUIDReply* response) override;
  // Gets the security level of the provider
  grpc::Status GetSecurityLevel(::grpc::ServerContext* context,
                                const GetSecurityLevelRequest* request,
                                GetSecurityLevelReply* response) override;
};

}  // namespace rpc
}  // namespace signing

#endif  // SIGNING_SERVER_GRPC_H_