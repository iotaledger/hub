// Copyright 2018 IOTA Foundation

#ifndef HUB_COMMANDS_PROCESS_TRANSFER_BATCH_H_
#define HUB_COMMANDS_PROCESS_TRANSFER_BATCH_H_

#include <string>
#include <map>

#include "hub/commands/command.h"

namespace hub {
namespace rpc {
class ProcessTransferBatchRequest;
class ProcessTransferBatchReply;
}  // namespace rpc

namespace cmd {

class ProcessTransferBatch
    : public Command<hub::rpc::ProcessTransferBatchRequest,
                     hub::rpc::ProcessTransferBatchReply> {
 public:
  using Command<hub::rpc::ProcessTransferBatchRequest,
                hub::rpc::ProcessTransferBatchReply>::Command;

  grpc::Status doProcess(
      const hub::rpc::ProcessTransferBatchRequest* request,
      hub::rpc::ProcessTransferBatchReply* response) noexcept override;

  /********************
   * No zero transfers
   * Zero summed batch
   * Funds Availability
   ********************/
  grpc::Status validateTransfers(
      const hub::rpc::ProcessTransferBatchRequest* request,
      const std::map<std::string, int64_t>& identifierToId) noexcept;

  const std::string name() override { return "ProcessTransferBatch"; }
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_PROCESS_TRANSFER_BATCH_H_
