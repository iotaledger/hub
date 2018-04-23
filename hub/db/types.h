#ifndef __HUB_DB_TYPES_H_
#define __HUB_DB_TYPES_H_

namespace hub {
  namespace db {
    enum class UserAddressBalanceReason {
      DEPOSIT = 0,
      SWEEP = 1
    };

    enum class HubAddressBalanceReason {
      INBOUND = 1,
      OUTBOUND = 2
    };

    enum class UserAccountBalanceReason {
      SWEEP = 0,
      BUY = 1,
      WITHDRAWAL_CANCEL = 2,
      WITHDRAWAL = 3,
      SELL = 4
    };
  }
}

#endif /* __HUB_DB_TYPES_H_ */
