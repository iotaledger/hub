// Copyright 2018 IOTA Foundation

#ifndef HUB_STATS_SESSION_H_
#define HUB_STATS_SESSION_H_

#include <cstdint>
#include <ostream>
#include <string>

#include <glog/logging.h>

#include "hub/crypto/types.h"

namespace hub {
class ClientSession {
 public:
  ClientSession()
      : _tag(hub::crypto::UUID::generate()),
        _str(rep_str()) {}

  ~ClientSession() { LOG(INFO) << *this << "destroyed."; }

  const std::string& tag() const { return _tag; }
  const std::string& to_str() const { return _str; }

 private:
  friend std::ostream& operator<<(std::ostream& os,
                                  const hub::ClientSession& stats);

  std::string rep_str() const {
    std::ostringstream ostr;
    ostr << "ClientSession(" << _tag << ") ";
    return ostr.str();
  }

  const std::string _tag;
  const std::string _str;
};

};  // namespace hub

#endif  // HUB_STATS_SESSION_H_
