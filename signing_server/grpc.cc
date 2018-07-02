/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "signing_server/grpc.h"
#include "signing_server/commands/get_address_for_uuid.h"
#include "signing_server/commands/get_signature_for_uuid.h"
#include "hub/crypto/provider.h"
#include "hub/stats/session.h"

#include <memory>
#include <string>

namespace hub {

namespace rpc {

namespace crypto {

// Gets the address for the UUID
grpc::Status SigningServerImpl::GetAddressForUUID(
    ::grpc::ServerContext* context,
    const rpc::crypto::GetAddressForUUIDRequest* request,
    rpc::crypto::GetAddressForUUIDReply* response) {
  auto clientSession = std::make_shared<ClientSession>();
  cmd::GetAddressForUUID cmd(clientSession);
  return cmd.process(request, response);
}
// Gets the signature for the UUID
grpc::Status SigningServerImpl::GetSignatureForUUID(
    ::grpc::ServerContext* context,
    const rpc::crypto::GetSignatureForUUIDRequest* request,
    rpc::crypto::GetSignatureForUUIDReply* response) {
  auto clientSession = std::make_shared<ClientSession>();
  cmd::GetSignatureForUUID cmd(clientSession);
  return cmd.process(request, response);
}
// Gets the security level of the provider
grpc::Status SigningServerImpl::GetSecurityLevel(
    ::grpc::ServerContext* context,
    const rpc::crypto::GetSecurityLevelRequest* request,
    GetSecurityLevelReply* response) {}
}  // namespace crypto
}  // namespace rpc

}  // namespace hub
