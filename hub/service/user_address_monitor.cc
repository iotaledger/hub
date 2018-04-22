#include "user_address_monitor.h"

#include "hub/db/db.h"
#include "hub/db/helper.h"

namespace hub {
namespace service {

std::vector<std::tuple<uint64_t, std::string>>
UserAddressMonitor::monitoredAddresses() {
  return hub::db::unsweptUserAddresses(hub::db::DBManager::get().connection());
}
}  // namespace service

}  // namespace hub
