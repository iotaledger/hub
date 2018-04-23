#ifndef __HUB_GRPC_H__
#define __HUB_GRPC_H__

#include <memory>

#include <grpc++/grpc++.h>

#include "proto/hub.grpc.pb.h"
#include "proto/hub.pb.h"

namespace hub {
class HubImpl final : public hub::rpc::Hub::Service {
 public:
  explicit HubImpl() {}
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
};

}  // namespace hub

#endif /* __HUB_GRPC_H__ */
