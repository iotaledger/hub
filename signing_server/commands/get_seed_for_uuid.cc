/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "get_seed_for_uuid.h"

#include "common/crypto/manager.h"
#include "proto/signing_server.pb.h"
#include "proto/signing_server_messages.pb.h"

#include "common/crypto/types.h"
#include "proto/messages.pb.h"
#include "signing_server/commands/helper.h"

namespace signing {
    namespace cmd {

        common::cmd::Error GetSeedForUUID::doProcess(
                const signing::rpc::GetSeedForUUIDRequest* request,
                signing::rpc::GetSeedForUUIDReply* response) noexcept {
            try {
                LOG(INFO) << session() << " GetSeedForUUID: " << request->uuid();

                common::crypto::UUID uuid(request->uuid());
                auto seed = common::crypto::CryptoManager::get()
                        .provider()
                        .getSeedFromUUID(uuid);

                response->set_seed(seed);
            } catch (const std::runtime_error& ex) {
                LOG(ERROR) << session() << "Failed: " << ex.what();

                return common::cmd::UNKNOWN_ERROR;
            }

            return common::cmd::OK;
        }

    }  // namespace cmd

}  // namespace signing
