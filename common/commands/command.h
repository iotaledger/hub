/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_COMMANDS_COMMAND_H_
#define COMMON_COMMANDS_COMMAND_H_

#include <string>

#include <boost/property_tree/ptree.hpp>

#include "common/commands/errors.h"
#include "common/stats/session.h"
#include "cppclient/api.h"

namespace common {
class ClientSession;

/// Command abstract class. This is the base class for all
/// command subclasses.
class ICommand {
 public:
  void setClientSession(std::shared_ptr<ClientSession> session) {
    _clientSession = std::move(session);
  }

  virtual bool needApi() const { return false; }

  virtual void setApi(std::shared_ptr<cppclient::IotaAPI> api){};

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

 protected:
  /// Get the shared client session
  /// @return ClientSession - a client session
  ClientSession& session() const { return *_clientSession; }

  /// Shared client session
  std::shared_ptr<ClientSession> _clientSession;

 private:
  /// Process the request - invokes the protected doProcess() implementation
  /// @param[in] request - contains the details of the request
  /// @return response - respons as a property tree
  virtual boost::property_tree::ptree doProcess(
      const boost::property_tree::ptree& request) noexcept = 0;
};

/// Command abstract class template.
/// Each concrete subclass is responsible for implementing
/// their respective behaviour for the correponding request to IRI
template <typename REQ, typename RES>
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
