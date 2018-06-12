# HIP-002: Checksum validation

## Problem description 
Exchanges need address's checksum in order to spend tokens from it when using wallet 
Also, when withdrawing, due to some errors, whether it is wrong address given 
by user or wrong use of hub on the exchange side, we wouldn't want the transaction 
to complete, to keep everyone happy.

## Proposed solution
**1.UserWithdraw will now validate provided address is valid using checksum.
2.GetDepositAddress will return the checksum**

```proto
// add `checksum` to GetDepositAddressReply:
message GetDepositAddressReply {
  // Newly created deposit address
  string address = 1;
  string checksum = 2;
}
// Provided address should contain checksum
message UserWithdrawRequest {
  string userId = 1;
  // Address the user requests payout to. Should be without checksum.
  string payoutAddressWithChecksum = 2;
  // Requested withdrawal amount
  uint64 amount = 3;
  // Tag for withdrawal
  string tag = 4;
}
```

