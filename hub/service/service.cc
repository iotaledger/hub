// Copyright 2018 IOTA Foundation

#include "hub/service/service.h"

#include <stdexcept>

#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>

namespace hub {
namespace service {

void Service::start() {
  if (_thread) {
    throw std::runtime_error{name() + "::start called but already running."};
  }

  _work = std::make_unique<boost::asio::io_service::work>(_service);
  _thread = std::make_unique<std::thread>(
      boost::bind(&boost::asio::io_service::run, &_service));

  onStart();
}

void Service::stop() {
  if (!_thread) {
    throw std::runtime_error{name() + "::stop called but was not running."};
  }

  onStop();

  _work = nullptr;
  _service.stop();
  _thread->join();
  _thread = nullptr;
}

}  // namespace service
}  // namespace hub
