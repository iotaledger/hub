/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/grpc_server_base.h"
#include "common/common.h"

#include <glog/logging.h>

namespace common {

void GrpcServerBase::runAndWait() { _server->Wait(); }

std::shared_ptr<grpc::ServerCredentials> GrpcServerBase::makeCredentials(
    const std::string& authMode, const std::string& sslCertPath,
    const std::string& sslKeyPath, const std::string& sslCAPath) {
  LOG(INFO) << "Using auth mode: " << authMode;
  if (authMode == "none") {
    return grpc::InsecureServerCredentials();
  } else if (authMode == "ssl") {
    grpc::SslServerCredentialsOptions options;

    grpc::SslServerCredentialsOptions::PemKeyCertPair keycert = {
        readFile(sslKeyPath), readFile(sslCertPath)};

    options.pem_key_cert_pairs.push_back(keycert);
    options.pem_root_certs = readFile(sslCAPath);
    options.client_certificate_request =
        GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY;
    return grpc::SslServerCredentials(options);
  }

  LOG(FATAL) << "Unknown auth mode: " << authMode;
}
}  // namespace common
