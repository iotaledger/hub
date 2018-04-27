// Copyright 2018 IOTA Foundation

#ifndef HUB_SERVICE_SWEEP_SERVICE_H_
#define HUB_SERVICE_SWEEP_SERVICE_H_

#include "hub/service/scheduled_service.h"

namespace hub {
namespace service {

class SweepService : public ScheduledService {
  using ScheduledService::ScheduledService;

 protected:
  bool doTick() override;
};

}  // namespace service
}  // namespace hub

#endif  // HUB_SERVICE_SWEEP_SERVICE_H_
