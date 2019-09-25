/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include <glog/logging.h>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "common/common.h"

#include "hub/crypto/remote_signing_provider.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

namespace hub {
namespace crypto {

RemoteSigningProvider::RemoteSigningProvider(const std::string& url,
                                             const std::string& certPath,
                                             const std::string& chainPath,
                                             const std::string& keyPath) {
  std::string cert = common::readFile(certPath);
  std::string chain = common::readFile(chainPath);
  std::string key = common::readFile(keyPath);
  grpc::SslCredentialsOptions opts;
  opts.pem_root_certs = cert;
  opts.pem_cert_chain = chain;
  opts.pem_private_key = key;

  auto credentials = grpc::SslCredentials(opts);

  auto channelSharedPtr = grpc::CreateChannel(url, credentials);
  _stub = signing::rpc::SigningServer::NewStub(channelSharedPtr);
}

/// Get a new address for a given UUID and the salt
/// param[in] UUID - a UUID
nonstd::optional<common::crypto::Address>
RemoteSigningProvider::getAddressForUUID(
    const common::crypto::UUID& uuid) const {
  ClientContext context;
  signing::rpc::GetAddressForUUIDRequest request;
  request.set_uuid(uuid.str());
  signing::rpc::GetAddressForUUIDReply response;

  Status status = _stub->GetAddressForUUID(&context, request, &response);
  if (!status.ok()) {
    std::cout << "GetAddressForUUID rpc failed:" << status.error_message();
    return {};
  }
  return {common::crypto::Address(response.address())};
}

/// The current security level
/// @return size_t - the security level (1 - 3)
nonstd::optional<size_t> RemoteSigningProvider::securityLevel(
    const common::crypto::UUID& uuid) const {
  ClientContext context;
  signing::rpc::GetSecurityLevelRequest request;
  signing::rpc::GetSecurityLevelReply response;
  request.set_uuid(uuid.str());

  Status status = _stub->GetSecurityLevel(&context, request, &response);
  if (!status.ok()) {
    LOG(ERROR) << "GetSecurityLevel rpc failed: " << status.error_message();
    return {};
  }
  return {response.securitylevel()};
}

/// Calculate the signature for a UUID and a bundle hash
/// param[in] UUID - a UUID
/// param[in] Hash - a bundleHash
/// @return string - the signature
nonstd::optional<std::string> RemoteSigningProvider::doGetSignatureForUUID(
    const common::crypto::UUID& uuid,
    const common::crypto::Hash& bundleHash) const {
  ClientContext context;
  signing::rpc::GetSignatureForUUIDRequest request;
  request.set_uuid(uuid.str());
  request.set_bundlehash(bundleHash.str());
  signing::rpc::GetSignatureForUUIDReply response;

  Status status = _stub->GetSignatureForUUID(&context, request, &response);
  if (!status.ok()) {
    LOG(ERROR) << "GetSignatureForUUID rpc failed:" << status.error_message();
    return {};
  }
  return {response.signature()};
}

}  // namespace crypto
}  // namespace hub
