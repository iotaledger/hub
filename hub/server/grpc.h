#ifndef __HUB_GRPC_H__
#define __HUB_GRPC_H__

#include <memory>

#include <grpc++/grpc++.h>

#include "proto/hub.grpc.pb.h"
#include "proto/hub.pb.h"

namespace iota {
class HubImpl final : public iota::rpc::Hub::Service {
 public:
  explicit HubImpl() {}
  ~HubImpl() override {}

  grpc::Status CreateUser(grpc::ServerContext* context,
                          const rpc::CreateUserRequest* request,
                          rpc::CreateUserReply* response) override;

#if 0
  grpc::Status GetBalance(grpc::ServerContext* context,
                          const iota::rpc::BalanceRequest* request,
                          iota::rpc::BalanceReply* response) override {
    return grpc::Status::CANCELLED;
  }
  grpc::Status GetDepositAddress(
      grpc::ServerContext* context,
      const iota::rpc::DepositAddressRequest* request,
      iota::rpc::DepositAddressReply* response) override {
    return grpc::Status::CANCELLED;
  }
  grpc::Status ScheduleSweep(grpc::ServerContext* context,
                             const iota::rpc::ScheduleSweepRequest* request,
                             iota::rpc::ScheduleSweepReply* response) override {
    return grpc::Status::CANCELLED;
  }
  grpc::Status UserWithdraw(grpc::ServerContext* context,
                            const iota::rpc::UserWithdrawRequest* request,
                            iota::rpc::UserWithdrawReply* response) override {
    return grpc::Status::CANCELLED;
  }
  grpc::Status GetStats(grpc::ServerContext* context,
                        const iota::rpc::StatsRequest* request,
                        iota::rpc::StatsReply* response) override {
    return grpc::Status::CANCELLED;
  }
  grpc::Status GetUserHistory(
      grpc::ServerContext* context,
      const iota::rpc::GetUserHistoryRequest* request,
      iota::rpc::GetUserHistoryReply* response) override {
    return grpc::Status::CANCELLED;
  }
  grpc::Status UserBalanceSubscription(
      grpc::ServerContext* context,
      const iota::rpc::UserBalanceSubscriptionRequest* request,
      grpc::ServerWriter<iota::rpc::UserBalanceEvent>* writer) override {
    return grpc::Status::CANCELLED;
  }
#endif
};

}  // namespace iota

#endif /* __HUB_GRPC_H__ */
