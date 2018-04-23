#include "grpc.h"

#include <memory>
#include <string>

#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>

#include "hub/commands/create_user.h"
#include "hub/commands/get_balance.h"
#include "hub/commands/get_deposit_address.h"
#include "hub/commands/user_withdraw.h"
#include "hub/stats/session.h"

namespace hub {

grpc::Status HubImpl::CreateUser(grpc::ServerContext* context,
                                 const rpc::CreateUserRequest* request,
                                 rpc::CreateUserReply* response) {
  auto clientSession = std::make_shared<ClientSession>();
  cmd::CreateUser cmd(clientSession);
  return cmd.process(request, response);
}

grpc::Status HubImpl::GetBalance(grpc::ServerContext* context,
                                 const rpc::GetBalanceRequest* request,
                                 rpc::GetBalanceReply* response) {
  auto clientSession = std::make_shared<ClientSession>();
  cmd::GetBalance cmd(clientSession);
  return cmd.process(request, response);
}

grpc::Status HubImpl::GetDepositAddress(
    grpc::ServerContext* context, const rpc::GetDepositAddressRequest* request,
    rpc::GetDepositAddressReply* response) {
  auto clientSession = std::make_shared<ClientSession>();
  cmd::GetDepositAddress cmd(clientSession);
  return cmd.process(request, response);
}

grpc::Status HubImpl::UserWithdraw(grpc::ServerContext* context,
                                   const hub::rpc::UserWithdrawRequest* request,
                                   hub::rpc::UserWithdrawReply* response) {
  auto clientSession = std::make_shared<ClientSession>();
  cmd::UserWithdraw cmd(clientSession);
  return cmd.process(request, response);
}

}  // namespace hub
