#include "helper.h"

#include <sstream>

#include "proto/hub.pb.h"

namespace iota {
namespace cmd {
std::string errorToString(const iota::rpc::ErrorCode& e) {
  std::ostringstream ss;
  iota::rpc::Error error;

  error.set_code(e);
  error.SerializeToOstream(&ss);

  return ss.str();
}
}  // namespace cmd
}  // namespace iota
