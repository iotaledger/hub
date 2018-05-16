// Copyright 2018 IOTA Foundation

#include "hub/commands/tests/helper.h"

namespace hub {

namespace tests {

std::map<uint64_t, int64_t> createZigZagTransfer(
    std::vector<std::string>& users, rpc::ProcessTransferBatchRequest& req,
    int64_t absAmount) {
  std::map<uint64_t, int64_t> idsToTransAmount;
  for (auto i = 0; i < users.size(); ++i) {
    auto* transfer = req.add_transfers();
    int64_t mul = (i % 2) ? 1 : -1;
    transfer->set_amount(mul * absAmount);
    transfer->set_userid(users[i]);
    idsToTransAmount[i] = mul * absAmount;
  }
  return idsToTransAmount;
}

void createBalanceForUsers(std::vector<uint64_t> ids, int64_t balance) {
  std::vector<hub::db::UserTransfer> transfers;
  for (auto id : ids) {
    transfers.emplace_back(hub::db::UserTransfer{id, balance});
  }

  auto& connection = db::DBManager::get().connection();
  connection.insertUserTransfers(transfers);
}

}  // namespace tests
}  // namespace hub
