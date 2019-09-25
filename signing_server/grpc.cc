/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "signing_server/grpc.h"
#include "common/crypto/provider_base.h"
#include "common/stats/session.h"
#include "signing_server/commands/get_address_for_uuid.h"
#include "signing_server/commands/get_security_level.h"
#include "signing_server/commands/get_signature_for_uuid.h"

#include <memory>
#include <string>

namespace signing {
namespace rpc {

// Gets the address for the UUID
grpc::Status SigningServerImpl::GetAddressForUUID(
    ::grpc::ServerContext* context, const GetAddressForUUIDRequest* request,
    GetAddressForUUIDReply* response) {
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd::GetAddressForUUID cmd(clientSession);
  return common::cmd::errorToGrpcError(cmd.process(request, response));
}
// Gets the signature for the UUID
grpc::Status SigningServerImpl::GetSignatureForUUID(
    ::grpc::ServerContext* context, const GetSignatureForUUIDRequest* request,
    GetSignatureForUUIDReply* response) {
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd::GetSignatureForUUID cmd(clientSession);
  return common::cmd::errorToGrpcError(cmd.process(request, response));
}
// Gets the security level of the provider
grpc::Status SigningServerImpl::GetSecurityLevel(
    ::grpc::ServerContext* context, const GetSecurityLevelRequest* request,
    GetSecurityLevelReply* response) {
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd::GetSecurityLevel cmd(clientSession);
  return common::cmd::errorToGrpcError(cmd.process(request, response));
}

}  // namespace rpc
}  // namespace signing
