#include "session.h"

namespace iota {
  std::ostream& operator<<(std::ostream& os, const iota::ClientSession& session) {
    os << session.to_str();
    return os;
  }
}
