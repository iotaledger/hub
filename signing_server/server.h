/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_CRYPTO_SERVER_H_
#define HUB_CRYPTO_SERVER_H_

#include <memory>
#include <string>

#include <grpc++/grpc++.h>
#include "common/server_base.h"
#include "signing_server/grpc.h"

namespace grpc {
class Server;
class ServerBuilder;
class ServerCredentials;
}  // namespace grpc

namespace hub {

namespace crypto {

/// SigningServer class.
/// The SigningServer holds the underlying logic which provides
/// Addresses/Signature/Seed generation based on a secret "salt" argument
class SigningServer : public common::ServerBase {
 public:
  /// Creates and initializes the API interface.
  void initialise() override;

 private:
  signing::rpc::SigningServerImpl _service;
};
}  // namespace crypto

}  // namespace hub

#endif  // HUB_CRYPTO_SERVER_H_
