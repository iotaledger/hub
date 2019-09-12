/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_COMMANDS_COMMAND_H_
#define COMMON_COMMANDS_COMMAND_H_

#include <memory>
#include <string>
#include <utility>

#include <glog/logging.h>
#include <boost/property_tree/ptree.hpp>

#include "common/commands/errors.h"
#include "common/stats/session.h"

namespace common {
class ClientSession;

class ICommand {
 public:
  void setClientSession(std::shared_ptr<ClientSession> session) {
    _clientSession = std::move(session);
  }

  /// constructor
  /// @param[in] session - the current client session
  explicit ICommand(std::shared_ptr<ClientSession> session)
      : _clientSession(std::move(session)) {}

  /// constructor
  ICommand() {}

  /// Process the request - invokes the protected doProcess() implementation
  /// @param[in] request - contains the details of the request
  /// @return response - respons as a property tree
  boost::property_tree::ptree process(
      const boost::property_tree::ptree& request) noexcept {
    return doProcess(request);
  }

  /// Process the request - invokes the protected doProcess() implementation
  /// @param[in] request - contains the details of the request
  /// @return response - respons as a property tree
  virtual boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept = 0;

 protected:
  /// Get the shared client session
  /// @return ClientSession - a client session
  ClientSession& session() const { return *_clientSession; }

  /// Shared client session
  std::shared_ptr<ClientSession> _clientSession;
};

template <typename REQ, typename RES>
/// Command abstract class template. This is the base class for all
/// command subclasses. Each concrete subclass is responsible for implementing
/// their respective behaviour for the correponding request to IRI
class Command : public ICommand {
 public:
  /// constructor
  /// @param[in] session - the current client session
  explicit Command(std::shared_ptr<ClientSession> session)
      : ICommand(std::move(session)) {}

  /// constructor
  Command() {}

  /// Process the request - invokes the protected doProcess() implementation
  /// @param[in] request - contains the details of the request
  /// @param[in] response - contains the details of the response
  /// @return common::cmd::Error
  common::cmd::Error process(const REQ* request, RES* response) noexcept {
    return doProcess(request, response);
  }

  /// Effectively process the request
  /// @param[in] request - contains the details of the request
  /// @param[in] response - contains the details of the response
  /// @return common::cmd::Error
  virtual common::cmd::Error doProcess(
      const REQ* request,
      RES* response) noexcept = 0;  /// Effectively process the request
};

}  // namespace common

#endif  // COMMON_COMMANDS_COMMAND_H_
