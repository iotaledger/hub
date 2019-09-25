/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_SERVER_GRPC_H_
#define HUB_SERVER_GRPC_H_

#include <memory>

#include <grpc++/grpc++.h>

#include "cppclient/api.h"
#include "proto/hub.grpc.pb.h"
#include "proto/hub.pb.h"

namespace hub {
/// HubImpl class.
/// The HubImpl class provides a server that listens and processes requests
/// from clients. The hub provides the following API:
/// - CreateUser. Creates a new user
/// - GetBalance. Get the balance for a given user
/// - GetDepositAddress. Get a new deposit address
/// - UserWithdraw. Process a withdrawal for a user
/// - UserWithdrawCancel. Cancels a previous withdrawal request (if not
/// processed)
/// - GetUserHistory. Get the history for a user
/// - UserBalanceSubscription. Monitors the balance for a user
/// - ProcessTransferBatch. Process transfers in batch
class HubImpl final : public hub::rpc::Hub::Service {
 public:
  /// Constructor
  HubImpl() {}
  /// Destructor
  ~HubImpl() override {}

  void setApi(std::shared_ptr<cppclient::IotaAPI> api);

  /// Creates a new user
  /// @param[in] context - server context
  /// @param[in] request - a rpc::CreateUserRequest request
  /// @param[in] response - a rpc::CreateUserReply response
  /// @return grpc::Status
  grpc::Status CreateUser(grpc::ServerContext* context,
                          const rpc::CreateUserRequest* request,
                          rpc::CreateUserReply* response) override;

  /// Get the balance for a given user
  /// @param[in] context - server context
  /// @param[in] request - a rpc::GetBalanceRequest request
  /// @param[in] response - a rpc::GetBalanceReply response
  /// @return grpc::Status
  grpc::Status GetBalance(grpc::ServerContext* context,
                          const hub::rpc::GetBalanceRequest* request,
                          hub::rpc::GetBalanceReply* response) override;

  /// Get a new deposit address
  /// @param[in] context - server context
  /// @param[in] request - a rpc::GetDepositAddressRequest request
  /// @param[in] response - a rpc::GetDepositAddressReply response
  /// @return grpc::Status
  grpc::Status GetDepositAddress(
      grpc::ServerContext* context,
      const hub::rpc::GetDepositAddressRequest* request,
      hub::rpc::GetDepositAddressReply* response) override;

  /// Process a withdrawal for a user
  /// @param[in] context - server context
  /// @param[in] request - a rpc::UserWithdrawRequest request
  /// @param[in] response - a rpc::UserWithdrawReply response
  /// @return grpc::Status
  grpc::Status UserWithdraw(grpc::ServerContext* context,
                            const hub::rpc::UserWithdrawRequest* request,
                            hub::rpc::UserWithdrawReply* response) override;

  /// Cancels a previous withdrawal request (if not processed)
  /// @param[in] context - server context
  /// @param[in] rpcRequest - a rpc::UserWithdrawCancelRequest request
  /// @param[in] rpcResponse - a rpc::UserWithdrawCancelReply response
  /// @return grpc::Status
  grpc::Status UserWithdrawCancel(
      grpc::ServerContext* context,
      const hub::rpc::UserWithdrawCancelRequest* rpcRequest,
      hub::rpc::UserWithdrawCancelReply* rpcResponse) override;

  /// Get the history for a user
  /// @param[in] context - server context
  /// @param[in] request - a rpc::GetUserHistoryRequest request
  /// @param[in] response - a rpc::GetUserHistoryReply response
  /// @return grpc::Status
  grpc::Status GetUserHistory(grpc::ServerContext* context,
                              const ::hub::rpc::GetUserHistoryRequest* request,
                              hub::rpc::GetUserHistoryReply* response) override;

  /// Monitors the balance for a user
  /// @param[in] context - server context
  /// @param[in] request - a rpc::BalanceSubscriptionRequest request
  /// @param[in] writer - a grpc::ServerWriter
  /// @return grpc::Status
  grpc::Status BalanceSubscription(
      grpc::ServerContext* context,
      const hub::rpc::BalanceSubscriptionRequest* request,
      grpc::ServerWriter<hub::rpc::BalanceEvent>* writer) override;

  /// Process transfers in batch
  /// @param[in] context - server context
  /// @param[in] request - a rpc::ProcessTransferBatchRequest request
  /// @param[in] response - a rpc::ProcessTransferBatchReply response
  /// @return grpc::Status
  grpc::Status ProcessTransferBatch(
      grpc::ServerContext* context,
      const hub::rpc::ProcessTransferBatchRequest* request,
      hub::rpc::ProcessTransferBatchReply* response) override;

  grpc::Status SweepSubscription(
      grpc::ServerContext* context,
      const hub::rpc::SweepSubscriptionRequest* request,
      grpc::ServerWriter<hub::rpc::SweepEvent>* writer) override;

  grpc::Status GetAddressInfo(grpc::ServerContext* context,
                              const ::hub::rpc::GetAddressInfoRequest* request,
                              hub::rpc::GetAddressInfoReply* response) override;

  grpc::Status SweepInfo(grpc::ServerContext* context,
                         const hub::rpc::SweepInfoRequest* request,
                         hub::rpc::SweepEvent* response) override;

  grpc::Status SignBundle(grpc::ServerContext* context,
                          const hub::rpc::SignBundleRequest* request,
                          hub::rpc::SignBundleReply* response) override;

  grpc::Status SweepDetail(grpc::ServerContext* context,
                           const hub::rpc::SweepDetailRequest* request,
                           hub::rpc::SweepDetailReply* response) override;

  grpc::Status GetStats(grpc::ServerContext* context,
                        const hub::rpc::GetStatsRequest* request,
                        hub::rpc::GetStatsReply* response) override;

  /// Returns true if withdrawal was cancelled
  /// @param[in] context - server context
  /// @param[in] request - a rpc::WasWithdrawalCancelledRequest request
  /// @param[in] response - a rpc::WasWithdrawalCancelledResponse response
  /// @return grpc::Status

  grpc::Status WasWithdrawalCancelled(
      grpc::ServerContext* context,
      const hub::rpc::WasWithdrawalCancelledRequest* request,
      hub::rpc::WasWithdrawalCancelledReply* response) override;

  /// Returns true if address was spent from
  /// @param[in] context - server context
  /// @param[in] request - a rpc::wasAddressSpentFromRequest request
  /// @param[in] response - a rpc::wasAddressSpentFromReply response
  /// @return grpc::Status

  grpc::Status WasAddressSpentFrom(
      grpc::ServerContext* context,
      const hub::rpc::WasAddressSpentFromRequest* request,
      hub::rpc::WasAddressSpentFromReply* response) override;

  /// Recover funds from an already spent address
  /// @param[in] context - server context
  /// @param[in] request - a rpc::RecoverFundsRequest request
  /// @param[in] response - a rpc::RecoverFundsReply response
  /// @return grpc::Status

  grpc::Status RecoverFunds(grpc::ServerContext* context,
                            const hub::rpc::RecoverFundsRequest* request,
                            hub::rpc::RecoverFundsReply* response) override;

 private:
  std::shared_ptr<cppclient::IotaAPI> _api;
};

}  // namespace hub

#endif  // HUB_SERVER_GRPC_H_
