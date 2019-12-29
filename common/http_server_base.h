/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_HTTP_SERVER_BASE_H_
#define COMMON_HTTP_SERVER_BASE_H_

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/websocket.hpp>
#include <string>

#include "server_base.h"

namespace common {

class HttpServerBase : public ServerBase {
 public:
  typedef enum { OK = 0, COMMAND_NOT_FOUND, WRONG_REQUEST_FORMAT } ErrorCode;
  /// Runs the service and waits for requests
  virtual void runAndWait() override;
  virtual ErrorCode handleRequestImpl(std::string_view request_body,
                                      std::string& response) = 0;
private:
    static void setupCredentials(boost::asio::ssl::context& ctx);
};
}  // namespace common

#endif  // COMMON_HTTP_SERVER_BASE_H_
