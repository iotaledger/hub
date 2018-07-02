#include "get_signature_for_uuid.h"

#include "hub/crypto/manager.h"
#include "proto/signing_server.pb.h"
#include "proto/signing_server_messages.pb.h"

#include "common/types/types.h"

namespace signing {
namespace cmd {

grpc::Status GetSignatureForUUID::doProcess(
    const signing::rpc::GetSignatureForUUIDRequest* request,
    signing::rpc::GetSignatureForUUIDReply* response) noexcept {
  try {
    LOG(INFO) << session() << " GetSignatureForUUID: " << request->uuid();

    common::crypto::UUID uuid(request->uuid());
    common::crypto::Hash bundleHash(request->bundlehash());
    auto signature =
        hub::crypto::CryptoManager::get().provider().forceGetSignatureForUUID(
            uuid, bundleHash);
    response->set_signature(signature);
  } catch (const std::runtime_error& ex) {
    LOG(ERROR) << session() << "Failed: " << ex.what();

    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "", "");
  }

  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace signing

//
// Created by tsvi on 6/27/18.
//
