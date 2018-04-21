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

#if 0
  grpc::Status ScheduleSweep(grpc::ServerContext* context,
                             const hub::rpc::ScheduleSweepRequest* request,
                             hub::rpc::ScheduleSweepReply* response) override {
    return grpc::Status::CANCELLED;
  }
  grpc::Status UserWithdraw(grpc::ServerContext* context,
                            const hub::rpc::UserWithdrawRequest* request,
                            hub::rpc::UserWithdrawReply* response) override {
    return grpc::Status::CANCELLED;
  }
  grpc::Status GetStats(grpc::ServerContext* context,
                        const hub::rpc::StatsRequest* request,
                        hub::rpc::StatsReply* response) override {
    return grpc::Status::CANCELLED;
  }
  grpc::Status GetUserHistory(
      grpc::ServerContext* context,
      const hub::rpc::GetUserHistoryRequest* request,
      hub::rpc::GetUserHistoryReply* response) override {
    return grpc::Status::CANCELLED;
  }
  grpc::Status UserBalanceSubscription(
      grpc::ServerContext* context,
      const hub::rpc::UserBalanceSubscriptionRequest* request,
      grpc::ServerWriter<hub::rpc::UserBalanceEvent>* writer) override {
    return grpc::Status::CANCELLED;
  }
#endif
};

}  // namespace hub

#endif /* __HUB_GRPC_H__ */
