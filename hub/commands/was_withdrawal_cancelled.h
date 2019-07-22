/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_WAS_WITHDRAWAL_CANCELLED_H_
#define HUB_COMMANDS_WAS_WITHDRAWAL_CANCELLED_H_

#include <string>

#include "common/command.h"

namespace hub {
    namespace rpc {
        class WasWithdrawalCancelledRequest;
        class WasWithdrawalCancelledReply;
    }  // namespace rpc

    namespace cmd {


        /// Returns true if withdrawal was cancelled
        /// @param[in] request - a rpc::WasWithdrawalCancelledRequest request
        /// @param[in] response - a rpc::WasWithdrawalCancelledResponse response
        class WasWithdrawalCancelled
                : public common::Command<hub::rpc::WasWithdrawalCancelledRequest,
                        hub::rpc::WasWithdrawalCancelledReply> {
        public:
            using Command<hub::rpc::WasWithdrawalCancelledRequest,
                    hub::rpc::WasWithdrawalCancelledReply>::Command;

            const std::string name() override { return "WasWithdrawalCancelled"; }

            grpc::Status doProcess(
                    const hub::rpc::WasWithdrawalCancelledRequest* request,
                    hub::rpc::WasWithdrawalCancelledReply* response) noexcept override;
        };
    }  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_WAS_WITHDRAWAL_CANCELLED_H_
