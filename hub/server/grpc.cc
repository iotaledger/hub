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

  cmd::CreateUser cmd(clientSession);
  
  cmd.process(request, response);
  
  std::string jsonReq;
  google::protobuf::util::MessageToJsonString(*request, &jsonReq, {});
  
  LOG(INFO) << "json: " << jsonReq;
  
  return grpc::Status::OK;
}
}  // namespace iota
