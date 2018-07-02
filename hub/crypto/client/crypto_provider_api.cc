/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "crypto_provider_api.h"
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

CryptoProviderApi::CryptoProviderApi(const std::string& url,
                                     const std::string& authMode,
                                     const std::string& certPath) {
  if (authMode == "none") {
    auto channelSharedPtr =
        grpc::CreateChannel(url, grpc::InsecureChannelCredentials());
    _stub = rpc::crypto::SigningServer::NewStub(channelSharedPtr);
  } else if (authMode == "ssl") {
    std::string cert = common::readFile(certPath);
    grpc::SslCredentialsOptions opts;
    opts.pem_root_certs = cert;

    auto credentials = grpc::SslCredentials(opts);

    auto channelSharedPtr = grpc::CreateChannel(url, credentials);
    _stub = rpc::crypto::SigningServer::NewStub(channelSharedPtr);
  } else {
    LOG(FATAL) << "Unknown auth mode: " << authMode;
  }
}

/// Get a new address for a given UUID and the salt
/// param[in] UUID - a UUID
Address CryptoProviderApi::getAddressForUUID(const UUID& uuid) const {
  ClientContext context;
  rpc::crypto::GetAddressForUUIDRequest request;
  request.set_uuid(uuid.str());
  rpc::crypto::GetAddressForUUIDReply response;

  Status status = _stub->GetAddressForUUID(&context, request, &response);
  if (!status.ok()) {
    std::cout << "getAddressForUUID rpc failed." << std::endl;
    return NULL_ADDRESS;
  }
  return hub::crypto::Address(response.address());
}

/// The current security level
/// @return size_t - the security level (1 - 3)
size_t CryptoProviderApi::securityLevel() const {
  ClientContext context;
  rpc::crypto::GetSecurityLevelRequest request;
  rpc::crypto::GetSecurityLevelReply response;

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
std::string CryptoProviderApi::doGetSignatureForUUID(
    const UUID& uuid, const Hash& bundleHash) const {
  ClientContext context;
  rpc::crypto::GetSignatureForUUIDRequest request;
  request.set_uuid(uuid.str());
  request.set_bundlehash(bundleHash.str());
  rpc::crypto::GetSignatureForUUIDReply response;

  Status status = _stub->GetSignatureForUUID(&context, request, &response);
  if (!status.ok()) {
    LOG(ERROR) << "GetSignatureForUUID rpc failed.";
  }
  return response.signature();
}

}  // namespace crypto
}  // namespace hub
