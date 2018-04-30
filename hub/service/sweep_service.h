// Copyright 2018 IOTA Foundation

#ifndef HUB_SERVICE_SWEEP_SERVICE_H_
#define HUB_SERVICE_SWEEP_SERVICE_H_

#include "hub/service/scheduled_service.h"

#include <string>

namespace hub {
namespace service {

class SweepService : public ScheduledService {
 public:
  using ScheduledService::ScheduledService;
  virtual ~SweepService() {}

  const std::string name() const override { return "SweepService"; }

 protected:
  bool doTick() override;
};

}  // namespace service
}  // namespace hub

#endif  // HUB_SERVICE_SWEEP_SERVICE_H_
