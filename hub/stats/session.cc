// Copyright 2018 IOTA Foundation

#include "hub/stats/session.h"

namespace hub {
std::ostream& operator<<(std::ostream& os, const hub::ClientSession& session) {
  os << session.to_str();
  return os;
}
}  // namespace hub
