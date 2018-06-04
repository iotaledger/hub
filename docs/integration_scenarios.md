# How to integrate RPCHub into your environment

Depending on how you are currently dealing with the management of user balances, there are different ways to use and integrate the RPCHub.
Below we'll discuss two possible scenarios:
- *Scenario A*: Multiple user accounts for generating deposit addresses, but funds are manually moved out of these upon deposit into a central `HOT` or `Hub` account.
- *Scenario B*: Multiple user accounts with individual balances.

## Scenario A
It might not be desired to rely on the Hub's internal accounting setup. One such reason might simply be that forwarding netted trades is too cumbersome.
Therefore, after a successful deposit, all funds might be transferred to a central `HOT` account. This will then also allow to deal with cold storage of funds easily: withdraw from this `HOT` account to a cold wallet address and deposit back into the account as necessary. User withdrawals are then also processed from this `HOT` account.

_Note that this means that the RPCHub does not track individual per-user balances. All per-user balances have to be tracked on the exchange side._

### Example flows
#### Initial setup
1. Exchange creates a `HOT` account

#### User signup
1. Exchange creates new Hub user, passing in a per-user userid.

#### User deposit
1. User requests deposit address for his user id (`GetDepositAddress`)
2. User deposits funds.
3. Exchange polls for new updates via `BalanceSubscription`.
4. Upon successful deposit (& sweep), exchange calls `ProcessTransfers`, transferring all new user deposits to the `HOT` account.

#### User withdrawal
1. User requests withdrawal on exchange frontend.
2. Exchange issues withdrawal from `HOT` to user address. (`UserWithdraw`)
3. Hub processes this withdrawal as part of a sweep.

#### Cold wallet topup
1. Exchange issues withdrawal from `HOT` to cold wallet address that wasn't spent from. (`UserWithdraw`)

  *Be aware that the Hub does _not_ check whether a payout address was already spent from! This has to happen on the exchange side.*

#### Hot wallet topup
1. Exchange requests deposit address for `HOT` user (`GetDepositAddress`)

   *Note that these addresses are single use! _Never_ send deposits to such a deposit address twice!*
2. Exchange sends funds from cold wallet to this deposit address
3. Hub receives deposit and moves to internal address as part of a sweep.

#### User B buys tokens from user A on exchange
1. No action happens on the Hub, all accounting is done internally on the Exchange side.

### Discussion of Pros & Cons
- (+) Easy management of cold / hot funds
- (+) Likely to be easier to integrate on exchange side.
- (-) Reduced security guarantees because balances are not tracked on a per-user level inside the Hub.
- ( ) Exchange needs to keep track of total amount of IOTA tokens independently of Hub.

## Scenario B
As the Hub supports fully independent user accounts with individual balances, it arguably makes esnse to rely on this as an added security measure. Balances are tracked per-user and therefore a user can only use as many funds as the user is tracking for them. However, this approach currently complicates the cold/hot wallet flow. 

### Cold/hot wallets
As opposed to Scenario A, it is not so easy to move funds from multiple users to a cold wallet. However, it is possible to have the Hub ignore certain internal addresses. For this, the `is_cold_storage` column on the `hub_address` table row needs to be set to 1. This will cause the `SweepService` to ignore this address for any sweeps.

For increased security, the `seed_uuid` of this hub address should also be deleted from the database and stored externally.

At the moment, the only way that this can be achieved is through manual database updates. It is recommended to stop the Hub while marking such hub addresses as cold storage. There is no negative effect on operations if Hub is stopped.

Using the `salt` that's passed at startup and the `seed_uuid` it is always possible to recompute the Hub address's seed outside of the Hub.

Should sufficient interest exist for this integration scenario, it is possible to provide specialised endpoints for this.

### Example flows
#### Initial setup
None. Start the Hub.

#### User signup
1. Exchange creates new Hub user, passing in a per-user userid.

#### User deposit
1. User requests deposit address for his user id (`GetDepositAddress`)
2. User deposits funds.
3. Hub moves the new deposit to an internal address.
3. Exchange polls for new updates via `BalanceSubscription` and notifies user on their frontend once the deposit has been registered or once it has been swept successfully.

#### User withdrawal
1. User requests withdrawal on exchange frontend.
2. Exchange issues withdrawal from user's Hub account to payout address (`UserWithdraw`)
3. Hub processes this withdrawal as part of a sweep.

#### Cold wallet topup
1. Exchange stops Hub.
2. Exchange decides which Hub addresses it wants to mark as cold storage.
3. Exchange sets `is_cold_storage` to `1` on these `hub_address` rows and stores the `seed_uuid` externally.
   There are multiple scenarios for achieving this:
   - Use a vault service.
   - Use paper backups
   - Some RDBMS support partitioning the table into multiple storage locations.
4. Exchange restarts Hub.
   
#### Hot wallet topup
1. Exchange decides which cold storage addresses it wants to reactivate.
2. Exchange stops Hub.
3. Exchange sets `is_cold_storage` to `0` on these `hub_address` rows and restores the `seed_uuid` values.
4. Exchange restarts hub.

#### User B buys tokens from user A on exchange
1. If not already exists, User B is created on Hub (`CreateUser`)
2. As part of next batch, exchange issues a transfer between the two users (`ProcessTransfers`)

### Discussion of Pros & Cons
- (+) Balances are tracked on a per-user level and thus Hub can do a sanity check on the requests the exchange sends.
- (+) Exchange can easily do a sanity check that its backend is tracking the same `(user, balance)` values as the Hub.
- (-) More complicated cold wallet setup
