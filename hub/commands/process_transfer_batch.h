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

#include "common/commands/command.h"

namespace hub {
namespace rpc {
class ProcessTransferBatchRequest;
class ProcessTransferBatchReply;
}  // namespace rpc

namespace cmd {

/// Invokes the processing of a batch of transfers.
/// @param[in] hub::rpc::ProcessTransferBatchRequest
/// @param[in] hub::rpc::ProcessTransferBatchReply
class ProcessTransferBatch
    : public common::Command<hub::rpc::ProcessTransferBatchRequest,
                             hub::rpc::ProcessTransferBatchReply> {
 public:
  using Command<hub::rpc::ProcessTransferBatchRequest,
                hub::rpc::ProcessTransferBatchReply>::Command;

  static std::shared_ptr<common::ICommand> create() {
    return std::shared_ptr<common::ICommand>(new ProcessTransferBatch());
  }

  static const std::string name() { return "ProcessTransferBatch"; }

  common::cmd::Error doProcess(
      const hub::rpc::ProcessTransferBatchRequest* request,
      hub::rpc::ProcessTransferBatchReply* response) noexcept override;

  boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept override;

 private:
  /// Checks the validity of a batch transfer. For a batch transfer to be valid,
  /// it must conform to the following criteria:
  /// - There must be no zero transfers
  /// - The sum of all transfers in the batch must amount to zero
  /// - Users must have sufficient funds available
  common::cmd::Error validateTransfers(
      const hub::rpc::ProcessTransferBatchRequest* request,
      const std::map<std::string, int64_t>& identifierToId) noexcept;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_PROCESS_TRANSFER_BATCH_H_
