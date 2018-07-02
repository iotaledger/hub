#include "get_security_level.h"

#include "hub/crypto/manager.h"
#include "proto/signing_server.pb.h"
#include "proto/signing_server_messages.pb.h"

#include "hub/crypto/types.h"

namespace hub {
namespace cmd {

grpc::Status GetSecurityLevel::doProcess(
    const rpc::crypto::GetSecurityLevelRequest* request,
    rpc::crypto::GetSecurityLevelReply* response) noexcept {
  try {
    LOG(INFO) << session() << " GetSecurityLevel";

    auto security = crypto::CryptoManager::get().provider().securityLevel();
    response->set_securitylevel(security);
  } catch (const std::runtime_error& ex) {
    LOG(ERROR) << session() << "Failed: " << ex.what();

    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "", "");
  }

  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace hub
