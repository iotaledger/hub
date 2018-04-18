#include "grpc.h"

#include <memory>
#include <string>

#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>

#include "hub/db/db.h"
#include "hub/stats/session.h"
#include "hub/commands/create_user.h"

namespace iota {

grpc::Status HubImpl::CreateUser(grpc::ServerContext* context,
                                 const rpc::CreateUserRequest* request,
                                 rpc::CreateUserReply* response) {
  auto clientSession = std::make_shared<ClientSession>();

  std::string jsonReq;
  google::protobuf::util::MessageToJsonString(*request, &jsonReq, {});
  LOG(INFO) << *clientSession << "request: " << jsonReq;

  cmd::CreateUser cmd(clientSession);
  
  return cmd.process(request, response);
}
}  // namespace iota
