/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/service/scheduled_service.h"

#include <iostream>
#include <memory>

#include <glog/logging.h>
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/bind.hpp>

namespace hub {
namespace service {

void ScheduledService::start() {
  if (_interval == std::chrono::milliseconds::zero()) {
    LOG(INFO) << " Not starting " << name() << " because it's disabled.";
    return;
  }

  Service::start();

  _timer = std::make_unique<Timer>(_service);

  _timer->expires_from_now(_interval);
  _timer->async_wait(boost::bind(&ScheduledService::tick, this));
}

void ScheduledService::tick() {
  VLOG(3) << name() << "::tick()";

  auto ret = doTick();

  if (ret) {
    _timer->expires_from_now(_interval);
    _timer->async_wait(boost::bind(&ScheduledService::tick, this));
  }
}

void ScheduledService::stop() {
  if (_interval == std::chrono::milliseconds::zero()) {
    LOG(INFO) << " Not stopping " << name() << " because it's disabled.";
    return;
  }

  _timer = nullptr;
  Service::stop();
}

}  // namespace service
}  // namespace hub
