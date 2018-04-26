// Copyright 2018 IOTA Foundation

#ifndef HUB_SERVICE_SERVICE_H_
#define HUB_SERVICE_SERVICE_H_

#include <memory>
#include <string>
#include <thread>

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>

namespace hub {
namespace service {

class Service {
 public:
  Service() {}

  virtual void start();
  virtual void stop();
  virtual bool isRunning() { return _thread != nullptr; }

  virtual const std::string name() const = 0;

 protected:
  virtual void onStart() {}
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
