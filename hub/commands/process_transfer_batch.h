/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */


#ifndef HUB_COMMANDS_PROCESS_TRANSFER_BATCH_H_
#define HUB_COMMANDS_PROCESS_TRANSFER_BATCH_H_

#include <map>
#include <string>

#include "hub/commands/command.h"

namespace hub {
namespace rpc {
class ProcessTransferBatchRequest;
class ProcessTransferBatchReply;
}  // namespace rpc

namespace cmd {

/// ProcessTransferBatch command. Invokes the processing of a batch of transfers.
/// Returns ???
/// Constructor
/// @param[in] hub::rpc::ProcessTransferBatchRequest
/// @param[in] hub::rpc::ProcessTransferBatchReply
class ProcessTransferBatch
    : public Command<hub::rpc::ProcessTransferBatchRequest,
                     hub::rpc::ProcessTransferBatchReply> {
 public:
  using Command<hub::rpc::ProcessTransferBatchRequest,
                hub::rpc::ProcessTransferBatchReply>::Command;

  const std::string name() override { return "ProcessTransferBatch"; }

 protected:
  grpc::Status doProcess(
      const hub::rpc::ProcessTransferBatchRequest* request,
      hub::rpc::ProcessTransferBatchReply* response) noexcept override;

 private:
  /// Checks the validity of a batch transfer. For a batch transfer to be valid,
  /// it must conform to the following criteria:
  /// - There must be no zero transfers
  /// - The sum of all transfers in the batch must amount to zero
  /// - Funds must be available to perform each transfer
  grpc::Status validateTransfers(
      const hub::rpc::ProcessTransferBatchRequest* request,
      const std::map<std::string, int64_t>& identifierToId) noexcept;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_PROCESS_TRANSFER_BATCH_H_
