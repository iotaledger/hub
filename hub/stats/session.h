#ifndef __HUB_STATS_SESSION_H__
#define __HUB_STATS_SESSION_H__

#include <cstdint>
#include <ostream>

#include <glog/logging.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace hub {
class ClientSession {
 public:
  explicit ClientSession()
      : _tag(boost::uuids::random_generator()()), _str(rep_str()) {}

  ~ClientSession() { LOG(INFO) << *this << "destroyed."; }

  const boost::uuids::uuid& tag() const { return _tag; }
  const std::string& to_str() const { return _str; }

 private:
  friend std::ostream& operator<<(std::ostream& os,
                                  const hub::ClientSession& stats);

  std::string rep_str() const {
    std::ostringstream ostr;
    ostr << "ClientSession(" << _tag << ") ";
    return ostr.str();
  }

  const boost::uuids::uuid _tag;
  const std::string _str;
};

};  // namespace hub

#endif
