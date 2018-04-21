#ifndef __HUB_SERVICE_SERVICE_H_
#define __HUB_SERVICE_SERVICE_H_

#include <thread>

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>

namespace hub {
namespace service {

class Service {
 public:
  explicit Service() {}

  virtual void start();
  virtual void stop();
  virtual bool isRunning() { return _thread != nullptr; }

  virtual const std::string name() const = 0;

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

#endif /*__HUB_SERVICE_SERVICE_H_ */
