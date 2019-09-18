/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "hub/server/grpc.h"

#include <memory>
#include <string>
#include <utility>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>

#include "common/stats/session.h"
#include "hub/commands/balance_subscription.h"
#include "hub/commands/converter.h"
#include "hub/commands/create_user.h"
#include "hub/commands/get_address_info.h"
#include "hub/commands/get_balance.h"
#include "hub/commands/get_deposit_address.h"
#include "hub/commands/get_stats.h"
#include "hub/commands/get_user_history.h"
#include "hub/commands/process_transfer_batch.h"
#include "hub/commands/recover_funds.h"
#include "hub/commands/sign_bundle.h"
#include "hub/commands/sweep_detail.h"
#include "hub/commands/sweep_info.h"
#include "hub/commands/sweep_subscription.h"
#include "hub/commands/user_withdraw.h"
#include "hub/commands/user_withdraw_cancel.h"
#include "hub/commands/was_address_spent_from.h"
#include "hub/commands/was_withdrawal_cancelled.h"

DEFINE_bool(SignBundle_enabled, false,
            "Whether the SignBundle API call should be available");

DEFINE_bool(RecoverFunds_enabled, false,
            "Whether the RecoverFunds API call should be available");

namespace hub {

void HubImpl::setApi(std::shared_ptr<cppclient::IotaAPI> api) {
  _api = std::move(api);
}

grpc::Status HubImpl::CreateUser(grpc::ServerContext* context,
                                 const rpc::CreateUserRequest* rpcRequest,
                                 rpc::CreateUserReply* rpcResponse) {
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd::CreateUser cmd(clientSession);
  cmd::CreateUserRequest req = {.userId = rpcRequest->userid()};
  cmd::CreateUserReply rep;
  return common::cmd::errorToGrpcError(cmd.process(&req, &rep));
}

grpc::Status HubImpl::GetBalance(grpc::ServerContext* context,
                                 const rpc::GetBalanceRequest* rpcRequest,
                                 rpc::GetBalanceReply* rpcResponse) {
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd::GetBalance cmd(clientSession);
  cmd::GetBalanceRequest req = {.userId = rpcRequest->userid()};
  cmd::GetBalanceReply rep;
  auto status = common::cmd::errorToGrpcError(cmd.process(&req, &rep));

  if (status.ok()) {
    rpcResponse->set_available(rep.available);
  }

  return status;
}

grpc::Status HubImpl::GetDepositAddress(
    grpc::ServerContext* context,
    const rpc::GetDepositAddressRequest* rpcRequest,
    rpc::GetDepositAddressReply* rpcResponse) {
  auto clientSession = std::make_shared<common::ClientSession>();

  cmd::GetDepositAddress cmd(clientSession);

  cmd::GetDepositAddressRequest req;
  req.userId = rpcRequest->userid();
  req.includeChecksum = rpcRequest->includechecksum();

  cmd::GetDepositAddressReply rep;

  auto status = common::cmd::errorToGrpcError(cmd.process(&req, &rep));

  if (status.ok()) {
    rpcResponse->set_address(rep.address);
  }

  return status;
}

grpc::Status HubImpl::UserWithdraw(
    grpc::ServerContext* context,
    const hub::rpc::UserWithdrawRequest* rpcRequest,
    hub::rpc::UserWithdrawReply* rpcResponse) {
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd::UserWithdraw cmd(clientSession, _api);
  cmd::UserWithdrawRequest request;
  cmd::UserWithdrawReply response;
  request.userId = rpcRequest->userid();
  request.amount = rpcRequest->amount();
  request.payoutAddress = rpcRequest->payoutaddress();
  request.validateChecksum = rpcRequest->validatechecksum();
  request.tag = rpcRequest->tag();

  auto status = common::cmd::errorToGrpcError(cmd.process(&request, &response));
  if (status.ok()) {
    rpcResponse->set_uuid(response.uuid);
  }

  return status;
}

grpc::Status HubImpl::UserWithdrawCancel(
    grpc::ServerContext* context,
    const hub::rpc::UserWithdrawCancelRequest* rpcRequest,
    hub::rpc::UserWithdrawCancelReply* rpcResponse) {
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd::UserWithdrawCancel cmd(clientSession);
  cmd::UserWithdrawCancelRequest request;
  cmd::UserWithdrawCancelReply response;

  request.uuid = rpcRequest->uuid();

  auto status = common::cmd::errorToGrpcError(cmd.process(&request, &response));
  rpcResponse->set_success(false);
  if (status.ok()) {
    rpcResponse->set_success(response.success);
  }
  return status;
}

grpc::Status HubImpl::GetUserHistory(
    grpc::ServerContext* context,
    const ::hub::rpc::GetUserHistoryRequest* rpcRequest,
    hub::rpc::GetUserHistoryReply* rpcResponse) {
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd::GetUserHistory cmd(clientSession);
  cmd::GetUserHistoryRequest request;
  cmd::GetUserHistoryReply reply;
  auto status = common::cmd::errorToGrpcError(cmd.process(&request, &reply));
  if (status.ok()) {
    for (const auto& e : reply.events) {
      auto* event = rpcResponse->add_events();
      event->set_userid(e.userId);
      event->set_timestamp(e.timestamp);
      event->set_amount(e.amount);
      event->set_type(cmd::userAccountBalanceEventTypeToProto(e.type));
      event->set_withdrawaluuid(e.withdrawalUUID);
      event->set_sweepbundlehash(e.sweepBundleHash);
    }
  }
  return status;
}

grpc::Status HubImpl::BalanceSubscription(
    grpc::ServerContext* context,
    const hub::rpc::BalanceSubscriptionRequest* request,
    grpc::ServerWriter<hub::rpc::BalanceEvent>* writer) {
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd::BalanceSubscription cmd(clientSession);
  return common::cmd::errorToGrpcError(cmd.process(request, writer));
}

grpc::Status HubImpl::ProcessTransferBatch(
    grpc::ServerContext* context,
    const hub::rpc::ProcessTransferBatchRequest* request,
    hub::rpc::ProcessTransferBatchReply* response) {
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd::ProcessTransferBatch cmd(clientSession);
  return common::cmd::errorToGrpcError(cmd.process(request, response));
}

grpc::Status HubImpl::SweepSubscription(
    grpc::ServerContext* context,
    const hub::rpc::SweepSubscriptionRequest* rpcRequest,
    grpc::ServerWriter<hub::rpc::SweepEvent>* writer) {
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd::SweepSubscription cmd(clientSession);

  cmd::SweepSubscriptionRequest request;
  std::vector<cmd::SweepEvent> sweepEvents;

  request.newerThan = rpcRequest->newerthan();
  auto status =
      common::cmd::errorToGrpcError(cmd.process(&request, &sweepEvents));

  if (status.ok()) {
    for (auto event : sweepEvents) {
      hub::rpc::SweepEvent rpcEvent;
      rpcEvent.set_bundlehash(event.bundleHash);
      rpcEvent.set_timestamp(event.timestamp);
      for (auto uuid : event.uuids) {
        rpcEvent.add_withdrawaluuid(uuid);
      }
      if (!writer->Write(rpcEvent)) {
        return grpc::Status::CANCELLED;
      }
    }
  }

  return status;
}

grpc::Status HubImpl::GetAddressInfo(
    grpc::ServerContext* context, const rpc::GetAddressInfoRequest* rpcRequest,
    rpc::GetAddressInfoReply* rpcResponse) {
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd::GetAddressInfoRequest request;
  cmd::GetAddressInfoReply response;
  cmd::GetAddressInfo cmd(clientSession);
  request.address = rpcRequest->address();
  auto status = common::cmd::errorToGrpcError(cmd.process(&request, &response));

  if (status.ok()) {
    rpcResponse->set_userid(response.userId);
  }

  return status;
}

grpc::Status HubImpl::SweepInfo(grpc::ServerContext* context,
                                const rpc::SweepInfoRequest* rpcRequest,
                                rpc::SweepEvent* rpcResponse) {
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd::SweepInfo cmd(clientSession);
  cmd::SweepInfoRequest request;
  cmd::SweepEvent response;
  if (rpcRequest->requestBy_case() ==
      hub::rpc::SweepInfoRequest::kWithdrawalUUID) {
    request.requestByUuid = true;
    request.uuid = rpcRequest->withdrawaluuid();
  } else {
    request.requestByUuid = false;
    request.bundleHash = rpcRequest->bundlehash();
  }
  auto status = common::cmd::errorToGrpcError(cmd.process(&request, &response));

  if (status.ok()) {
    rpcResponse->set_bundlehash(response.bundleHash);
    rpcResponse->set_timestamp(response.timestamp);
    for (auto uuid : response.uuids) {
      rpcResponse->add_withdrawaluuid(uuid);
    }
  }
  return status;
}

grpc::Status HubImpl::SignBundle(grpc::ServerContext* context,
                                 const hub::rpc::SignBundleRequest* request,
                                 hub::rpc::SignBundleReply* response) {
  auto clientSession = std::make_shared<common::ClientSession>();

  if (!FLAGS_SignBundle_enabled) {
    LOG(ERROR) << clientSession << ": SignBundle is disabled";
    return grpc::Status::CANCELLED;
  }

  cmd::SignBundle cmd(clientSession);
  return common::cmd::errorToGrpcError(cmd.process(request, response));
}

grpc::Status HubImpl::SweepDetail(grpc::ServerContext* context,
                                  const hub::rpc::SweepDetailRequest* request,
                                  hub::rpc::SweepDetailReply* response) {
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd::SweepDetail cmd(clientSession);
  return common::cmd::errorToGrpcError(cmd.process(request, response));
}

grpc::Status HubImpl::GetStats(grpc::ServerContext* context,
                               const hub::rpc::GetStatsRequest* rpcRequest,
                               hub::rpc::GetStatsReply* rpcResponse) {
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd::GetStats cmd(clientSession);
  hub::cmd::GetStatsRequest request;
  hub::cmd::GetStatsReply response;
  auto status = common::cmd::errorToGrpcError(cmd.process(&request, &response));
  if (status.ok()) {
    rpcResponse->set_totalbalance(response.totalBalance);
  }
  return status;
}

grpc::Status HubImpl::WasWithdrawalCancelled(
    grpc::ServerContext* context,
    const hub::rpc::WasWithdrawalCancelledRequest* rpcRequest,
    hub::rpc::WasWithdrawalCancelledReply* rpcResponse) {
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd::WasWithdrawalCancelled cmd(clientSession);
  cmd::WasWithdrawalCancelledRequest request;
  cmd::WasWithdrawalCancelledReply response;
  request.uuid = rpcRequest->uuid();
  auto status = common::cmd::errorToGrpcError(cmd.process(&request, &response));
  if (status.ok()) {
    rpcResponse->set_wascancelled(response.wasWihdrawalCancelled);
  }
  return status;
}

grpc::Status HubImpl::WasAddressSpentFrom(
    grpc::ServerContext* context,
    const hub::rpc::WasAddressSpentFromRequest* rpcRequest,
    hub::rpc::WasAddressSpentFromReply* rpcResponse) {
  auto clientSession = std::make_shared<common::ClientSession>();
  cmd::WasAddressSpentFrom cmd(clientSession);
  cmd::WasAddressSpentFromRequest request;
  cmd::WasAddressSpentFromReply response;
  request.address = rpcRequest->address();
  request.validateChecksum = rpcRequest->validatechecksum();
  auto status = common::cmd::errorToGrpcError(cmd.process(&request, &response));
  if (status.ok()) {
    rpcResponse->set_wasaddressspentfrom(response.wasAddressSpentFrom);
  }
  return status;
}

grpc::Status HubImpl::RecoverFunds(grpc::ServerContext* context,
                                   const hub::rpc::RecoverFundsRequest* request,
                                   hub::rpc::RecoverFundsReply* response) {
  auto clientSession = std::make_shared<common::ClientSession>();

  if (!FLAGS_RecoverFunds_enabled) {
    LOG(ERROR) << clientSession << ": Recover funds is disabled";
    return grpc::Status::CANCELLED;
  }

  cmd::RecoverFunds cmd(clientSession, _api);
  return common::cmd::errorToGrpcError(cmd.process(request, response));
}

}  // namespace hub
