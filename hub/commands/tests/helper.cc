// Copyright 2018 IOTA Foundation

#include "hub/commands/tests/helper.h"

namespace hub {

namespace tests {

std::map<uint64_t, int64_t> createZigZagTransfer(
    std::vector<std::string>& users, cmd::ProcessTransferBatchRequest& req,
    int64_t absAmount) {
  std::map<uint64_t, int64_t> idsToTransAmount;
  for (uint32_t i = 0; i < users.size(); ++i) {
    int64_t mul = (i % 2) ? 1 : -1;
    req.transfers.emplace_back(
        cmd::UserTransfer{userId : users[i], amount : mul * absAmount});
    idsToTransAmount[i + 1] = mul * absAmount;
  }
  return idsToTransAmount;
}

std::map<uint64_t, int64_t> createBalanceForUsers(std::vector<uint64_t> ids,
                                                  int64_t balance) {
  std::vector<hub::db::UserTransfer> transfers;
  std::map<uint64_t, int64_t> idsToBalances;
  for (auto id : ids) {
    transfers.emplace_back(hub::db::
                           UserTransfer{userId : id, amount : balance});
    idsToBalances[id] = balance;
  }

  auto& connection = db::DBManager::get().connection();
  connection.insertUserTransfers(transfers);
  return idsToBalances;
}

}  // namespace tests
}  // namespace hub
