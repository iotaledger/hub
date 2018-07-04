/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitcommon.com/iotaledger/rpccommon
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_COMMANDS_COMMAND_H_
#define COMMON_COMMANDS_COMMAND_H_

#include <memory>
#include <string>
#include <utility>

#include <glog/logging.h>
#include <grpc++/grpc++.h>

#include "common/stats/session.h"

namespace common {
class ClientSession;

namespace cmd {

template <typename REQ, typename RES>
/// Command abstract class template. This is the base class for all
/// command subclasses. Each concrete subclass is responsible for implementing
/// their respective behaviour for the correponding request to IRI
class Command {
 public:
  /// constructor
  /// @param[in] session - the current client session
  explicit Command(std::shared_ptr<ClientSession> session)
      : _clientSession(std::move(session)) {}

  /// Process the request - invokes the protected doProcess() implementation
  /// @param[in] request - contains the details of the request
  /// @param[in] response - contains the details of the response
  /// @return grpc::Status
  grpc::Status process(const REQ* request, RES* response) noexcept {
    VLOG(3) << *_clientSession << name() << "::process()";

    return doProcess(request, response);
  }

  /// Returns the descriptive name of the command
  /// @return std::string
  virtual const std::string name() = 0;

  /// Effectively process the request
  /// @param[in] request - contains the details of the request
  /// @param[in] response - contains the details of the response
  /// @return grpc::Status
  virtual grpc::Status doProcess(const REQ* request,
                                 RES* response) noexcept = 0;

 protected:
  /// Get the shared client session
  /// @return ClientSession - a client session
  ClientSession& session() const { return *_clientSession; }

  /// Shared client session
  const std::shared_ptr<ClientSession> _clientSession;
};

}  // namespace cmd
}  // namespace common

#endif  // COMMON_COMMANDS_COMMAND_H_
