/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */


#ifndef HUB_SERVER_GRPC_H_
#define HUB_SERVER_GRPC_H_

#include <memory>

#include <grpc++/grpc++.h>

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
/// - UserWithdrawCancel. Cancels a previous withdrawal request (if not processed)
/// - GetUserHistory. Get the history for a user
/// - UserBalanceSubscription. Monitors the balance for a user
/// - ProcessTransferBatch. Process transfers in batch
class HubImpl final : public hub::rpc::Hub::Service {
 public:
  /// Constructor
  HubImpl() {}
  /// Destructor
  ~HubImpl() override {}

  /// Creates a new user
  /// \param[in] context - server context
  /// \param[in] request - a rpc::CreateUserRequest request
  /// \param[in] response - a rpc::CreateUserReply response
  /// @return grpc::Status
  grpc::Status CreateUser(grpc::ServerContext* context,
                          const rpc::CreateUserRequest* request,
                          rpc::CreateUserReply* response) override;

  /// Get the balance for a given user
  /// \param[in] context - server context
  /// \param[in] request - a rpc::GetBalanceRequest request
  /// \param[in] response - a rpc::GetBalanceReply response
  /// @return grpc::Status
  grpc::Status GetBalance(grpc::ServerContext* context,
                          const hub::rpc::GetBalanceRequest* request,
                          hub::rpc::GetBalanceReply* response) override;

  /// Get a new deposit address
  /// \param[in] context - server context
  /// \param[in] request - a rpc::GetDepositAddressRequest request
  /// \param[in] response - a rpc::GetDepositAddressReply response
  /// @return grpc::Status
  grpc::Status GetDepositAddress(
      grpc::ServerContext* context,
      const hub::rpc::GetDepositAddressRequest* request,
      hub::rpc::GetDepositAddressReply* response) override;

  /// Process a withdrawal for a user
  /// \param[in] context - server context
  /// \param[in] request - a rpc::UserWithdrawRequest request
  /// \param[in] response - a rpc::UserWithdrawReply response
  /// @return grpc::Status
  grpc::Status UserWithdraw(grpc::ServerContext* context,
                            const hub::rpc::UserWithdrawRequest* request,
                            hub::rpc::UserWithdrawReply* response) override;

  /// Cancels a previous withdrawal request (if not processed)
  /// \param[in] context - server context
  /// \param[in] request - a rpc::UserWithdrawCancelRequest request
  /// \param[in] response - a rpc::UserWithdrawCancelReply response
  /// @return grpc::Status
  grpc::Status UserWithdrawCancel(
      grpc::ServerContext* context,
      const hub::rpc::UserWithdrawCancelRequest* request,
      hub::rpc::UserWithdrawCancelReply* response) override;

  /// Get the history for a user
  /// \param[in] context - server context
  /// \param[in] request - a rpc::GetUserHistoryRequest request
  /// \param[in] response - a rpc::GetUserHistoryReply response
  /// @return grpc::Status
  grpc::Status GetUserHistory(grpc::ServerContext* context,
                              const ::hub::rpc::GetUserHistoryRequest* request,
                              hub::rpc::GetUserHistoryReply* response) override;

  /// Monitors the balance for a user
  /// \param[in] context - server context
  /// \param[in] request - a rpc::UserBalanceSubscriptionRequest request
  /// \param[in] writer - a grpc::ServerWriter
  /// @return grpc::Status
  grpc::Status UserBalanceSubscription(
      grpc::ServerContext* context,
      const hub::rpc::UserBalanceSubscriptionRequest* request,
      grpc::ServerWriter<hub::rpc::UserBalanceEvent>* writer) override;

  /// Process transfers in batch
  /// \param[in] context - server context
  /// \param[in] request - a rpc::ProcessTransferBatchRequest request
  /// \param[in] response - a rpc::ProcessTransferBatchReply response
  /// @return grpc::Status
  grpc::Status ProcessTransferBatch(
      grpc::ServerContext* context,
      const hub::rpc::ProcessTransferBatchRequest* request,
      hub::rpc::ProcessTransferBatchReply* response) override;
};

}  // namespace hub

#endif  // HUB_SERVER_GRPC_H_
