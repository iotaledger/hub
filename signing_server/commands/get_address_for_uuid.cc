#include "get_address_for_uuid.h"

#include "hub/crypto/manager.h"
#include "proto/signing_server.pb.h"
#include "proto/signing_server_messages.pb.h"

#include "hub/crypto/types.h"

namespace hub {
namespace cmd {

grpc::Status GetAddressForUUID::doProcess(
    const rpc::crypto::GetAddressForUUIDRequest* request,
    rpc::crypto::GetAddressForUUIDReply* response) noexcept {
  try {
    LOG(INFO) << session() << " GetAddressForUUID: " << request->uuid();

    hub::crypto::UUID uuid(request->uuid());
    auto address =
        crypto::CryptoManager::get().provider().getAddressForUUID(uuid);
    response->set_address(address.str());
  } catch (const std::runtime_error& ex) {
    LOG(ERROR) << session() << "Failed: " << ex.what();

    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "", "");
  }

  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace hub
