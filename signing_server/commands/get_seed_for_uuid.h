/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef SIGNING_SERVER_COMMANDS_GET_SEED_FOR_UUID_H_
#define SIGNING_SERVER_COMMANDS_GET_SEED_FOR_UUID_H_

#include <string>

#include "common/commands/command.h"

namespace signing {
    namespace rpc {
        class GetSeedForUUIDRequest;
        class GetSeedForUUIDReply;
    }  // namespace rpc

    namespace cmd {

/// Gets information on an uuid
/// @param[in] common::rpc::GetSeedForUUIDRequest
/// @param[in] common::rpc::GetSeedForUUIDReply
        class GetSeedForUUID
                : public common::Command<signing::rpc::GetSeedForUUIDRequest,
                        signing::rpc::GetSeedForUUIDReply> {
        public:
            using common::Command<signing::rpc::GetSeedForUUIDRequest,
                    signing::rpc::GetSeedForUUIDReply>::Command;

            static const std::string name() { return "GetSeedForUUID"; }

            common::cmd::Error doProcess(
                    const signing::rpc::GetSeedForUUIDRequest* request,
                    signing::rpc::GetSeedForUUIDReply* response) noexcept override;

            boost::property_tree::ptree doProcess(
                    const boost::property_tree::ptree& request) noexcept override {return boost::property_tree::ptree();};
        };
    }  // namespace cmd
}  // namespace signing

#endif  // SIGNING_SERVER_COMMANDS_GET_SEED_FOR_UUID_H_
