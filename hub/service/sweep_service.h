/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_SERVICE_SWEEP_SERVICE_H_
#define HUB_SERVICE_SWEEP_SERVICE_H_

#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "common/crypto/types.h"
#include "cppclient/api.h"
#include "hub/db/types.h"
#include "hub/service/scheduled_service.h"

namespace hub {
namespace service {

/// SweepService class.
/// The sweep service is responsible for consolidating all the deposit and
/// widthdrawal transactions into hub addresses
class SweepService : public ScheduledService {
 public:
  /// constructor
  /// @param[in] api - an cppclient::IotaAPI compliant API provider
  /// @param[in] interval - the tick interval in milliseconds
  explicit SweepService(std::shared_ptr<cppclient::IotaAPI> api,
                        std::chrono::milliseconds interval)
      : ScheduledService(interval), _api(std::move(api)) {}

  /// Destructor
  virtual ~SweepService() {}

  /// @return string - the descriptive name of the service
  const std::string name() const override { return "SweepService"; }

  /// Persist the bundle data to database
  /// identified during the sweep.
  /// @param[in] bundle - the bundle hash and its serialized value
  /// @param[in] deposits - a list of deposit transactions
  /// @param[in] hubInputs - a list of internal transfers
  /// @param[in] withdrawals - a list of withdrawal transactions
  /// @param[in] hubOutput - the hub address into which the remainder is
  /// deposited
  void persistToDatabase(std::tuple<common::crypto::Hash, std::string> bundle,
                         const std::vector<db::TransferInput>& deposits,
                         const std::vector<db::TransferInput>& hubInputs,
                         const std::vector<db::TransferOutput>& withdrawals,
                         const db::TransferOutput& hubOutput);

  /// Creates a new hub address to which funds that remain after a transfer
  /// can be moved.
  /// @param[in] remainder
  /// @return a db::TransferOutput structure containing
  /// - the new hub address
  /// - the id of the new hub address
  /// - the remainder
  db::TransferOutput getHubOutput(uint64_t remainder);

 protected:
  /// Called by tick() by default. Override in subclasses
  /// @return false if it wants to stop.
  bool doTick() override;
  /// an cppclient::IotaAPI compliant API provider
  const std::shared_ptr<cppclient::IotaAPI> _api;
};

}  // namespace service
}  // namespace hub

#endif  // HUB_SERVICE_SWEEP_SERVICE_H_
