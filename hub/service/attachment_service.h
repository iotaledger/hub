/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_SERVICE_ATTACHMENT_SERVICE_H_
#define HUB_SERVICE_ATTACHMENT_SERVICE_H_

#include <chrono>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "cppclient/api.h"
#include "hub/db/db.h"
#include "hub/iota/pow.h"
#include "hub/service/scheduled_service.h"

namespace hub {
namespace db {
struct Sweep;
struct SweepTail;
}  // namespace db

namespace service {

/// AttachmentService class.
class AttachmentService : public ScheduledService {
 public:
  /// constructor
  /// @param[in] api - an cppclient::IotaAPI compliant API provider
  /// @param[in] interval - the tick interval in milliseconds
  explicit AttachmentService(std::shared_ptr<cppclient::IotaAPI> api,
                             std::chrono::milliseconds interval)
      : ScheduledService(interval), _api(std::move(api)) {}
  /// Destructor
  virtual ~AttachmentService() {}

  /// @return string - the descriptive name of the service
  const std::string name() const override { return "AttachmentService"; }

 protected:
  /// Called by tick() by default. Override in subclasses
  /// @return false if it wants to stop.
  bool doTick() override;

 protected:
  void reattachSweep(db::Connection& connection,
                     const iota::POWProvider& powProvider,
                     const db::Sweep& sweep);

  void promoteSweep(db::Connection& connection,
                    const iota::POWProvider& powProvider,
                    const db::Sweep& sweep,
                    const common::crypto::Hash& tailHash);

  bool checkForUserReattachment(db::Connection& connection,
                                const db::Sweep& sweep,
                                const std::vector<std::string>& knownTails);

  bool checkSweepTailsForConfirmation(db::Connection& connection,
                                      const db::Sweep& sweep,
                                      const std::vector<std::string>& tails);

 protected:
  /// an cppclient::IotaAPI compliant API provider
  const std::shared_ptr<cppclient::IotaAPI> _api;
};

}  // namespace service
}  // namespace hub

#endif  // HUB_SERVICE_ATTACHMENT_SERVICE_H_
