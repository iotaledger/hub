/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_SERVER_HTTP_SERVER_H_
#define HUB_SERVER_HTTP_SERVER_H_

#include <memory>
#include <string>

#include "common/http_server_base.h"
#include "cppclient/api.h"
#include "hub/server/hub_server_base.h"
#include "hub/service/attachment_service.h"
#include "hub/service/sweep_service.h"
#include "hub/service/user_address_monitor.h"

namespace hub {

/// HubServer class.
/// The HubServer runs the local services and holds the instance of the
/// cppclient::IotaAPI complient API interface to the tangle. It also holds
/// an instance of the hub service that will accept requests
class HubHttpJsonServer : public common::HttpServerBase, public HubServerBase {
 public:
  /// Constructor.
  HubHttpJsonServer();

  /// Creates and initializes the services and the API interface.
  void initialize() override;
  virtual ErrorCode handleRequestImpl(std::string_view request_body,
                                      std::string& response) override;

 private:
};

}  // namespace hub
#endif  // HUB_SERVER_HTTP_SERVER_H_
