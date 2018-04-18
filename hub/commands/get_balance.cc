#include "get_balance.h"

#include <sqlpp11/connection.h>
#include <sqlpp11/select.h>
#include <sqlpp11/functions.h>

#include "proto/hub.pb.h"
#include "schema/schema.h"

#include "hub/db/db.h"
#include "hub/stats/session.h"

#include "helper.h"
using namespace sqlpp;

namespace iota {
namespace cmd {

grpc::Status GetBalance::doProcess(const iota::rpc::GetBalanceRequest* request,
                                   iota::rpc::GetBalanceReply* response) noexcept {
  db::sql::UserAccount acc;
  db::sql::UserAddressBalance bal;

  auto& conn = db::DBManager::get().connection();

  auto userId = conn(select(acc.id).from(acc).where(acc.identifier == request->userid())).first();
  
  LOG(ERROR) << userId;
  


  return grpc::Status::OK;
}

}  // namespace cmd

}  // namespace iota
