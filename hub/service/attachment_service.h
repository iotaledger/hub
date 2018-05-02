// Copyright 2018 IOTA Foundation

#ifndef HUB_SERVICE_ATTACHMENT_SERVICE_H_
#define HUB_SERVICE_ATTACHMENT_SERVICE_H_

#include <chrono>
#include <memory>
#include <string>
#include <utility>

#include "hub/iota/api.h"
#include "hub/service/scheduled_service.h"

namespace hub {
namespace service {

class AttachmentService : public ScheduledService {
 public:
  explicit AttachmentService(std::shared_ptr<hub::iota::IotaAPI> api,
                             std::chrono::milliseconds interval)
      : ScheduledService(interval), _api(std::move(api)) {}
  virtual ~AttachmentService() {}

  const std::string name() const override { return "AttachmentService"; }

 protected:
  bool doTick() override;

 protected:
  const std::shared_ptr<hub::iota::IotaAPI> _api;
};

}  // namespace service
}  // namespace hub

#endif  // HUB_SERVICE_ATTACHMENT_SERVICE_H_
