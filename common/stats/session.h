/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_STATS_SESSION_H_
#define COMMON_STATS_SESSION_H_

#include <cstdint>
#include <ostream>
#include <string>

#include <glog/logging.h>

#include "common/crypto/types.h"

namespace common {
class ClientSession {
 public:
  ClientSession() : _tag(crypto::UUID().str()), _str(rep_str()) {}

  ~ClientSession() { LOG(INFO) << *this << "destroyed."; }

  const std::string& tag() const { return _tag; }
  const std::string& to_str() const { return _str; }

 private:
  friend std::ostream& operator<<(std::ostream& os, const ClientSession& stats);

  std::string rep_str() const {
    std::ostringstream ostr;
    ostr << "ClientSession(" << _tag << ") ";
    return ostr.str();
  }

  const std::string _tag;
  const std::string _str;
};

};  // namespace common

#endif  // COMMON_STATS_SESSION_H_
