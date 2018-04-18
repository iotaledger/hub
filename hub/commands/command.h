#ifndef __HUB_COMMANDS_COMMAND_H_
#define __HUB_COMMANDS_COMMAND_H_

#include <memory>

#include <glog/logging.h>
#include <grpc++/grpc++.h>

namespace iota {
class ClientSession;

namespace cmd {

template <typename REQ, typename RES>
class Command {
 public:
  explicit Command(std::shared_ptr<ClientSession> session)
      : _clientSession(std::move(session)) {}

  grpc::Status process(const REQ* request, RES* response) {
    VLOG(3) << *_clientSession << name() << "::process()";

    return doProcess(request, response);
  }

  virtual const std::string name() = 0;

 protected:
  virtual grpc::Status doProcess(const REQ* request, RES* response) = 0;
  ClientSession& session() const { return *_clientSession; }

  const std::shared_ptr<ClientSession> _clientSession;
};

}  // namespace cmd
}  // namespace iota

#endif /* __HUB_COMMANDS_COMMAND_H_ */
