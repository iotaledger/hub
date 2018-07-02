#include "get_signature_for_uuid.h"

#include "hub/crypto/manager.h"
#include "proto/signing_server.pb.h"
#include "proto/signing_server_messages.pb.h"

#include "hub/crypto/types.h"

namespace hub {
namespace cmd {

grpc::Status GetSignatureForUUID::doProcess(
    const rpc::crypto::GetSignatureForUUIDRequest* request,
    rpc::crypto::GetSignatureForUUIDReply* response) noexcept {
  try {
    LOG(INFO) << session() << " GetSignatureForUUID: " << request->uuid();

    hub::crypto::UUID uuid(request->uuid());
    hub::crypto::Hash bundleHash(request->bundlehash());
    auto signature =
        crypto::CryptoManager::get().provider().forceGetSignatureForUUID(
            uuid, bundleHash);
    response->set_signature(signature);
  } catch (const std::runtime_error& ex) {
    LOG(ERROR) << session() << "Failed: " << ex.what();

    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "", "");
  }

  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace hub

//
// Created by tsvi on 6/27/18.
//
