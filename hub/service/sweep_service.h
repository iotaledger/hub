// Copyright 2018 IOTA Foundation

#ifndef HUB_SERVICE_SWEEP_SERVICE_H_
#define HUB_SERVICE_SWEEP_SERVICE_H_

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

#include "hub/crypto/types.h"
#include "hub/db/types.h"
#include "hub/service/scheduled_service.h"

namespace hub {
namespace service {

class SweepService : public ScheduledService {
 public:
  using ScheduledService::ScheduledService;
  virtual ~SweepService() {}

  const std::string name() const override { return "SweepService"; }

 protected:
  bool doTick() override;

  db::TransferOutput getHubOutput(uint64_t remainder);
  std::tuple<hub::crypto::Hash, std::string> createBundle(
      const std::vector<db::TransferInput>& deposits,
      const std::vector<db::TransferInput>& hubInputs,
      const std::vector<db::TransferOutput>& withdrawals,
      const db::TransferOutput& hubOutput);

  void persistToDatabase(std::tuple<hub::crypto::Hash, std::string> bundle,
                         const std::vector<db::TransferInput>& deposits,
                         const std::vector<db::TransferInput>& hubInputs,
                         const std::vector<db::TransferOutput>& withdrawals,
                         const db::TransferOutput& hubOutput);
};

}  // namespace service
}  // namespace hub

#endif  // HUB_SERVICE_SWEEP_SERVICE_H_
