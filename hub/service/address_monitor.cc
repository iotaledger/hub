#include "address_monitor.h"

#include <stdexcept>

#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>

namespace hub {
namespace service {

bool AddressMonitor::doTick() { return true; }

}  // namespace service
}  // namespace hub
