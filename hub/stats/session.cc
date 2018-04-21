#include "session.h"

namespace hub {
  std::ostream& operator<<(std::ostream& os, const hub::ClientSession& session) {
    os << session.to_str();
    return os;
  }
}
