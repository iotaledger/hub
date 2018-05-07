// Copyright 2018 IOTA Foundation

#ifndef HUB_SERVER_GRPC_H_
#define HUB_SERVER_GRPC_H_

#include <memory>

#include <grpc++/grpc++.h>

#include "proto/hub.grpc.pb.h"
#include "proto/hub.pb.h"

namespace hub {
class HubImpl final : public hub::rpc::Hub::Service {
 public:
  HubImpl() {}
  ~HubImpl() override {}

  grpc::Status CreateUser(grpc::ServerContext* context,
                          const rpc::CreateUserRequest* request,
                          rpc::CreateUserReply* response) override;

  grpc::Status GetBalance(grpc::ServerContext* context,
                          const hub::rpc::GetBalanceRequest* request,
                          hub::rpc::GetBalanceReply* response) override;

  grpc::Status GetDepositAddress(
      grpc::ServerContext* context,
      const hub::rpc::GetDepositAddressRequest* request,
      hub::rpc::GetDepositAddressReply* response) override;

  grpc::Status UserWithdraw(grpc::ServerContext* context,
                            const hub::rpc::UserWithdrawRequest* request,
                            hub::rpc::UserWithdrawReply* response) override;

  grpc::Status UserWithdrawCancel(
      grpc::ServerContext* context,
      const hub::rpc::UserWithdrawCancelRequest* request,
      hub::rpc::UserWithdrawCancelReply* response) override;

  grpc::Status GetUserHistory(grpc::ServerContext* context,
                              const ::hub::rpc::GetUserHistoryRequest* request,
                              hub::rpc::GetUserHistoryReply* response) override;

  grpc::Status UserBalanceSubscription(
      grpc::ServerContext* context,
      const hub::rpc::UserBalanceSubscriptionRequest* request,
      grpc::ServerWriter<hub::rpc::UserBalanceEvent>* writer);
};

}  // namespace hub

#endif  // HUB_SERVER_GRPC_H_
