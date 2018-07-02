#include "get_address_for_uuid.h"

#include "hub/crypto/manager.h"
#include "proto/signing_server.pb.h"
#include "proto/signing_server_messages.pb.h"

#include "hub/crypto/types.h"
#include "proto/messages.pb.h"
#include "signing_server/commands/helper.h"

namespace signing {
namespace cmd {

grpc::Status GetAddressForUUID::doProcess(
    const signing::rpc::GetAddressForUUIDRequest* request,
    signing::rpc::GetAddressForUUIDReply* response) noexcept {
  try {
    LOG(INFO) << session() << " GetAddressForUUID: " << request->uuid();

    hub::crypto::UUID uuid(request->uuid());
    auto address =
        hub::crypto::CryptoManager::get().provider().getAddressForUUID(uuid);
    response->set_address(address.str());
  } catch (const std::runtime_error& ex) {
    LOG(ERROR) << session() << "Failed: " << ex.what();

    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                        errorToString(signing::rpc::ErrorCode::EC_UNKNOWN));
  }

  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace signing
