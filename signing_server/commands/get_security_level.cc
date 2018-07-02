#include "get_security_level.h"

#include "hub/crypto/manager.h"
#include "proto/signing_server.pb.h"
#include "proto/signing_server_messages.pb.h"

#include "common/types/types.h"

namespace signing {
namespace cmd {

grpc::Status GetSecurityLevel::doProcess(
    const signing::rpc::GetSecurityLevelRequest* request,
    signing::rpc::GetSecurityLevelReply* response) noexcept {
  try {
    LOG(INFO) << session() << " GetSecurityLevel";

    auto security = hub::crypto::CryptoManager::get().provider().securityLevel(
        common::crypto::UUID(request->uuid()));
    response->set_securitylevel(security);
  } catch (const std::runtime_error& ex) {
    LOG(ERROR) << session() << "Failed: " << ex.what();

    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "", "");
  }

  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace signing
