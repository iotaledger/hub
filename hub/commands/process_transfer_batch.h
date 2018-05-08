// Copyright 2018 IOTA Foundation

#ifndef HUB_COMMANDS_PROCESS_TRANSFER_BATCH_H_
#define HUB_COMMANDS_PROCESS_TRANSFER_BATCH_H_

#include <string>

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

  const std::string name() override { return "ProcessTransferBatch"; }
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_PROCESS_TRANSFER_BATCH_H_
