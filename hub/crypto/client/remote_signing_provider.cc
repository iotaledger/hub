/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "remote_signing_provider.h"
#include <glog/logging.h>
#include "common/common.h"

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

namespace hub {
namespace crypto {

RemoteSigningProvider::RemoteSigningProvider(const std::string& url,
                                             const std::string& authMode,
                                             const std::string& certPath,
                                             const std::string& chainPath,
                                             const std::string& keyPath) {
  if (authMode == "none") {
    auto channelSharedPtr =
        grpc::CreateChannel(url, grpc::InsecureChannelCredentials());
    _stub = signing::rpc::SigningServer::NewStub(channelSharedPtr);
  } else if (authMode == "ssl") {
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
  } else {
    LOG(FATAL) << "Unknown auth mode: " << authMode;
  }
}

/// Get a new address for a given UUID and the salt
/// param[in] UUID - a UUID
    common::crypto::Address RemoteSigningProvider::getAddressForUUID(const common::crypto::UUID& uuid) const {
  ClientContext context;
  signing::rpc::GetAddressForUUIDRequest request;
  request.set_uuid(uuid.str());
  signing::rpc::GetAddressForUUIDReply response;

  Status status = _stub->GetAddressForUUID(&context, request, &response);
  if (!status.ok()) {
    std::cout << "getAddressForUUID rpc failed." << std::endl;
    return NULL_ADDRESS;
  }
  return common::crypto::Address(response.address());
}

/// The current security level
/// @return size_t - the security level (1 - 3)
size_t RemoteSigningProvider::securityLevel(const common::crypto::UUID& uuid) const {
  ClientContext context;
  signing::rpc::GetSecurityLevelRequest request;
  signing::rpc::GetSecurityLevelReply response;

  Status status = _stub->GetSecurityLevel(&context, request, &response);
  if (!status.ok()) {
    LOG(ERROR) << "GetSecurityLevel rpc failed.";
    return 0;
  }
  return response.securitylevel();
}

/// Calculate the signature for a UUID and a bundle hash
/// param[in] UUID - a UUID
/// param[in] Hash - a bundleHash
/// @return string - the signature
std::string RemoteSigningProvider::doGetSignatureForUUID(
    const common::crypto::UUID& uuid, const common::crypto::Hash& bundleHash) const {
  ClientContext context;
  signing::rpc::GetSignatureForUUIDRequest request;
  request.set_uuid(uuid.str());
  request.set_bundlehash(bundleHash.str());
  signing::rpc::GetSignatureForUUIDReply response;

  Status status = _stub->GetSignatureForUUID(&context, request, &response);
  if (!status.ok()) {
    LOG(ERROR) << "GetSignatureForUUID rpc failed.";
  }
  return response.signature();
}

}  // namespace crypto
}  // namespace hub
