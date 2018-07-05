#include "get_signature_for_uuid.h"

#include "common/crypto/manager.h"
#include "proto/signing_server.pb.h"
#include "proto/signing_server_messages.pb.h"

#include "common/crypto/types.h"

namespace signing {
namespace cmd {

grpc::Status GetSignatureForUUID::doProcess(
    const signing::rpc::GetSignatureForUUIDRequest* request,
    signing::rpc::GetSignatureForUUIDReply* response) noexcept {
  try {
    LOG(INFO) << session() << " GetSignatureForUUID - uuid: " << request->uuid()<<" bundle hash: "<<request->bundlehash();

    common::crypto::UUID uuid(request->uuid());
    common::crypto::Hash bundleHash(request->bundlehash());

    response->set_signature(common::crypto::CryptoManager::get()
                                .provider()
                                .forceGetSignatureForUUID(uuid, bundleHash)
                                .value());
  } catch (const std::runtime_error& ex) {
    LOG(ERROR) << session() << "Failed: " << ex.what();

    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "", "");
  }

  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace crypto

//
// Created by tsvi on 6/27/18.
//
