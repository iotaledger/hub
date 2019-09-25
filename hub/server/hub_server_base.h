/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_SERVER_HUB_SERVER_H_
#define HUB_SERVER_HUB_SERVER_H_

#include <memory>
#include <string>

#include <gflags/gflags.h>
#include "cppclient/api.h"
#include "hub/service/attachment_service.h"
#include "hub/service/sweep_service.h"
#include "hub/service/user_address_monitor.h"

DECLARE_string(apiAddress);

DECLARE_uint32(monitorInterval);
DECLARE_uint32(attachmentInterval);
DECLARE_uint32(sweepInterval);

DECLARE_uint32(minWeightMagnitude);
DECLARE_uint32(depth);

DECLARE_string(hmacKeyPath);
DECLARE_string(authProvider);

// remote crypto provider settings
DECLARE_string(signingMode);
DECLARE_string(signingProviderAddress);
// The following credentials components has to match those of the SigningServer
DECLARE_string(signingServerSslCert);
DECLARE_string(signingServerChainCert);
DECLARE_string(signingServerKeyCert);

// remote/local pow settings
DECLARE_string(powMode);

// Extended
DECLARE_bool(fetchTransactionMessages);

DECLARE_bool(useHttpsIRI);

// GRPC/Http Rest
DECLARE_string(serverType);

namespace hub {

/// HubServer class.
/// The HubServer runs the local services and holds the instance of the
/// cppclient::IotaAPI complient API interface to the tangle. It also holds
/// an instance of the hub service that will accept requests
class HubServerBase {
 public:
  /// Constructor.
  HubServerBase();

  /// Creates and initializes the services and the API interface.
  void initialize_services();

 protected:
  std::shared_ptr<cppclient::IotaAPI> _api;

 private:
  std::unique_ptr<hub::service::UserAddressMonitor> _userAddressMonitor;
  std::unique_ptr<hub::service::AttachmentService> _attachmentService;
  std::unique_ptr<hub::service::SweepService> _sweepService;

  /// Verifies that the salt provided on the command line has the correct
  /// value for this server. Will always succeed if no user addresses exist
  /// in the database.
  /// @return bool - true on success
  bool authenticateSalt() const;

  void initialiseAuthProvider() const;
};

}  // namespace hub
#endif  // HUB_SERVER_HUB_SERVER_H_
