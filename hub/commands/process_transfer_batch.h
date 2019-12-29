/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_PROCESS_TRANSFER_BATCH_H_
#define HUB_COMMANDS_PROCESS_TRANSFER_BATCH_H_

#include <map>
#include <string>
#include <vector>

#include "common/commands/command.h"

namespace hub {

namespace cmd {

typedef struct UserTransfer {
  std::string userId;
  int64_t amount;
} UserTransfer;

typedef struct ProcessTransferBatchRequest {
  std::vector<UserTransfer> transfers;
} ProcessTransferBatchRequest;
typedef struct ProcessTransferBatchReply {
} ProcessTransferBatchReply;

/// Invokes the processing of a batch of transfers.
/// @param[in] ProcessTransferBatchRequest
/// @param[in] ProcessTransferBatchReply

class ProcessTransferBatch : public common::Command<ProcessTransferBatchRequest,
                                                    ProcessTransferBatchReply> {
 public:
  using Command<ProcessTransferBatchRequest,
                ProcessTransferBatchReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(
        new ProcessTransferBatch(std::make_shared<common::ClientSession>()));
  }

  static const std::string name() { return "ProcessTransferBatch"; }

  common::cmd::Error doProcess(
      const ProcessTransferBatchRequest* request,
      ProcessTransferBatchReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;

 private:
  /// Checks the validity of a batch transfer. For a batch transfer to be valid,
  /// it must conform to the following criteria:
  /// - There must be no zero transfers
  /// - The sum of all transfers in the batch must amount to zero
  /// - Users must have sufficient funds available
  common::cmd::Error validateTransfers(
      const ProcessTransferBatchRequest* request,
      const std::map<std::string, int64_t>& identifierToId) noexcept;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_PROCESS_TRANSFER_BATCH_H_
