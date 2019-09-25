/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef SIGNING_SERVER_SERVER_H_
#define SIGNING_SERVER_SERVER_H_

#include <memory>
#include <string>

#include <grpc++/grpc++.h>
#include "common/grpc_server_base.h"
#include "signing_server/grpc.h"

namespace signing {

namespace crypto {

/// SigningServer class.
/// The SigningServer holds the underlying logic which provides
/// Addresses/Signature/Seed generation based on a secret "salt" argument
class SigningServer : public common::GrpcServerBase {
 public:
  /// Creates and initializes the API interface.
  void initialize() override;

 private:
  signing::rpc::SigningServerImpl _service;
};
}  // namespace crypto

}  // namespace signing

#endif  // SIGNING_SERVER_SERVER_H_
