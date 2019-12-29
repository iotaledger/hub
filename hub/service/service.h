/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_SERVICE_SERVICE_H_
#define HUB_SERVICE_SERVICE_H_

#include <memory>
#include <string>
#include <thread>

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>

namespace hub {
namespace service {

/// Service abstract class. This is the base class for all services
/// Each concrete subclass is responsible for implementing their
/// respective behaviour
class Service {
 public:
  /// constructor
  Service() {}

  /// Start the service
  virtual void start();
  /// Stop the service
  virtual void stop();
  /// @return bool - true if the service is currently running, false otherwise
  virtual bool isRunning() { return _thread != nullptr; }

  /// @return string - the descriptive name of the service
  virtual const std::string name() const = 0;

 protected:
  /// Called immediately after starting the service
  virtual void onStart() {}
  /// Called immediately before stopping the service
  virtual void onStop() {}

  // Order of destruction matters for the below
 private:
  std::unique_ptr<std::thread> _thread;

 protected:
  boost::asio::io_service _service;
  boost::asio::io_service::strand _strand{_service};

 private:
  std::unique_ptr<boost::asio::io_service::work> _work;
};

}  // namespace service

}  // namespace hub

#endif  // HUB_SERVICE_SERVICE_H_
