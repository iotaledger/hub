# IOTA Hub: RESTFull API.

## Commands

### GetStats

- Get's the total balance managed by hub


- curl example:
    - Call:
        
        ``` curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "GetStats"}' ```
    
    - Output:
    ``` "totalBalance": "1000" ```
    

### CreateUser

- Creates a new user

- Arguments

|Arg name    |type          |Description                    |
|------------|--------------|-------------------------------|
| userId     |string        | unique user id                |

- curl example:
    - Call:
        
     ```
     curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "CreateUser", "userId": "SomeUser"}' 
     ```
    
    - Output:
    ```{}```
    
    
### GetDepositAddress

- Get's a random address for user to deposit funds into (will be monitored by Hub monitoring_service)

- Arguments

|Arg name    |type          |Description                    |
|------------|--------------|-------------------------------|
| userId     |string        | The user's identifier         |

- curl example:
    - Call:
        
     ```
     curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "GetDepositAddress", "userId": "SomeUser"}' 
     ```
    
    - Output:
    ``` {"address": "DILTSLPCVZTLRICPK9PZPAZOTKVUSAAYPOYHMLIIQKPASAGXHGAEZPGGEQGQZCKAHNKPTNIXJPFC9LOCZ"} ```
 

### GetBalance

- Get's the balance for a user

- Arguments

|Arg name    |type          |Description                    |
|------------|--------------|-------------------------------|
| userId     |string        |  The user's identifier        |

- curl example:
    - Call:
        
    ``` 
    curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "GetBalance", "userId": "SomeUser"}' 
    ```
    
    - Output:
    ````{"available": "1000"}````
    
 
### GetAddressInfo

- Get's the userId for a given address

- Arguments

|Arg name    |type              |Description                    |
|------------|------------------|-------------------------------|
| address    |string (81 trytes)| The address in question       |

- curl example:
    - Call:
        
    ``` 
    curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "GetAddressInfo", "address": "DILTSLPCVZTLRICPK9PZPAZOTKVUSAAYPOYHMLIIQKPASAGXHGAEZPGGEQGQZCKAHNKPTNIXJPFC9LOCZ"}' 
    ```
    
    - Output:
    ``` {"userId": "SomeUSer"} ```
    
    
    
### GetUserHistory

- Returns a list of all user balance events

- Arguments

|Arg name    |type          |Description                                         |
|------------|--------------|----------------------------------------------------|
| userId     |string        |  The user's identifier                             |
| newerThan  |timestamp     | all returned events are newer than given timestamp,|
|            |              | _timestamp_ _=_ _0_ means everything is retrieved  |

- curl example:
    - Call:
        
     ``` 
     curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "GetUserHistory", "userId": "SomeUser"}' 
     ```
    
    - Output:
    ```
    {
        "event_0": {
            "userID": "SomeUser",
            "timestamp": "1563796442000",
            "amount": "1000",
            "reason": "DEPOSIT",
            "sweepBundleHash": "EWLCW9FFLSBUGZZOOLFQLTRJFKNGPUVCIOCQYTSDOSZLBCBJIIJZGPALGAKKANBTDYOBVQFOJHA9OVFOY",
            "withdrawalUUID": ""
        },
        "event_1": {
            "userID": "SomeUser",
            "timestamp": "1563796562000",
            "amount": "-1",
            "reason": "WITHDRAWAL",
            "sweepBundleHash": "",
            "withdrawalUUID": "4782e7d5-9ce4-477d-8fd0-32f5f3385db2"
        },
        "event_2": {
            "userID": "SomeUser",
            "timestamp": "1563796604000",
            "amount": "1",
            "reason": "WITHDRAWAL_CANCELED",
            "sweepBundleHash": "",
            "withdrawalUUID": "4782e7d5-9ce4-477d-8fd0-32f5f3385db2"
        },

    }
    ```
    
    The list of possible user balance event "reason" are under "hub/commands/converter.cc"
    
    
### SweepDetail

- Returns details about a bundle status (confirmed?), transactions (trytes) and reattachment (tailHash)

- Arguments

|Arg name    |type              |Description                               |
|------------|------------------|------------------------------------------|
| bundleHash |string (81 trytes)| The hash of the bundle in question       |

- curl example:
    - Call:
        
    ``` 
    curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "SweepDetail", "bundleHash" : "EWLCW9FFLSBUGZZOOLFQLTRJFKNGPUVCIOCQYTSDOSZLBCBJIIJZGPALGAKKANBTDYOBVQFOJHA9OVFOY"} 
    ```
    
    - Output:
    ``` 
    { 
            "confirmed": "false",
             "trytes": "SOMETRANSACTIONTRYTES....",
             "trytes": "...SOMETRANSACTIONTRYTES.",
              ....
             "tailHash": "G9POQLBHRQZOZUV9XEROHSANPAIMHUQORSFQFRJM9JGUXHOHORDFWNTNUDWDCKXPUPJKXZDEBHXDDN999",
             "tailHash": "EOBRJDZTYTAXIMSEPXUWPZJBRMPJENHCFXLJGSHGIESTCQOGSGQLNOKTDYE9VKCAAGGINGKTEDYWYD999",
             "tailHash": "ATVRPYELRHPUHBAL9CSTQEJFEQCSENDNGF9AXHEFCYFJTAMQFUPPPPSVFXXXHPTQCAJJCEYJGVBARW999",
             ..... (more tails)
    } 
    ```
     
     
### SweepInfo

- Provide some information about a bundle

- Arguments

|Arg name        |type              |Description                                                    |
|----------------|------------------|---------------------------------------------------------------|
| bundleHash     |string (81 trytes)| The hash of the bundle in question                            |
| requestByUuid  |boolean           | whether bundle should be queried by a withdrawal UUID    
|                |                  | or by the bundle's hash
| withdrawalUuid |string            | the withdrawal UUID to provide when `requestByUuid` is "true"

- curl example:
    - Call:
        
     ``` 
     curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "SweepDetail", "bundleHash" : "EWLCW9FFLSBUGZZOOLFQLTRJFKNGPUVCIOCQYTSDOSZLBCBJIIJZGPALGAKKANBTDYOBVQFOJHA9OVFOY"} 
     ```
    
    - Output:
    ```
    {
            "bundleHash": "LFABJNKAKJVXYH9OPVZ9HJFOPOHDAGKOHZSRWHSNXYBHCYWQDHGRVKPFBLSGRZUOBL9DUBCKI9DWSPEJC",
            "timestamp": "1567577007000",
            "withdrawalUuid": "4782e7d5-9ce4-477d-8fd0-32f5f3385db2"
            "withdrawalUuid": "50485062-f5f0-4cac-bad6-bb0362ae5138"
    } 
    
    ```
     
### UserWithdraw

- Requests for a withdrawal of funds for user, if request succeeds, a bundle will be created

- Arguments

|Arg name            |type                 |Description                                                    |
|--------------------|---------------------|---------------------------------------------------------------|
| userId             |string               | The user's identifier
| amount             |integer              | The amount to withdraw    
| payoutAddress      |string (81/90 trytes)| The address to transfer the funds (_amount_) to
| validateChecksum   |bool                 | Should set to true if _payoutAddress_ 
|                    |                     | is 90 trytes long (81 + 9 checksum trytes)
| tag                |string (27 trytes)   | Optional tag to include on the withdrawal 
|                    |                     | transactions


- curl example:
    - Call:
        
    ``` 
    curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "UserWithdraw" , "userId" : "SomeUser", "amount": 1, "payoutAddress""LFABJNKAKJVXYH9OPVZ9HJFOPOHDAGKOHZSRWHSNXYBHCYWQDHGRVKPFBLSGRZUOBL9DUBCKI9DWSPEJC"}' 
    ```
    
    - Output:
    ```
    {
        "uuid": "50485062-f5f0-4cac-bad6-bb0362ae5138"
    }

    ```
     
### UserWithdrawCancel

- Requests for withdrawal cancellation

- Arguments

|Arg name            |type                 |Description                                         |
|--------------------|---------------------|----------------------------------------------------|
| uuid               |string               | The _uuid_ output of previous _UserWithdraw_ call



- curl example:
    - Call:
        
    ``` 
    curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "UserWithdrawCancel" , "uuid" : "50485062-f5f0-4cac-bad6-bb0362ae5138"}'  
    ```
    
    - Output:
    
    Upon success:
    ```
    {
       "success": "true"
    }
       
    ```
     When withdrawal has already been processed or cancelled:
     
    ```
    {
        "error": "Withdrawal can not be cancelled"
    }
    ```
     
### WasWithdrawalCancelled

- Queries the cancellation status of withdrawal

- Arguments

|Arg name            |type                 |Description                                         |
|--------------------|---------------------|----------------------------------------------------|
| uuid               |string               | The _uuid_ output of previous _UserWithdraw_ call



- curl example:
    - Call:
        
    ``` 
    curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "WasWithdrawalCancelled" , "uuid" : "c10e6d8f-1f7c-4fdc-b21d-2e533870be6e"}' 
    ```
    
    - Output:

    ```
    {
       "wasCancelled": "true"
    }
       
    ```
     
### WasAddressSpentFrom

- Return true when the address in question was already spent (should not be used again)

- Arguments

|Arg name            |type                 |Description                                                    |
|--------------------|---------------------|---------------------------------------------------------------|
| address            |string (81/90 trytes)| The address to transfer the funds (_amount_) to
| validateChecksum   |bool                 | Should set to true if _address_ 
|                    |                     | is 90 trytes long (81 + 9 checksum trytes)



- curl example:
    - Call:
        
    ``` 
    curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "WasAddressSpentFrom" , "address" : "LFABJNKAKJVXYH9OPVZ9HJFOPOHDAGKOHZSRWHSNXYBHCYWQDHGRVKPFBLSGRZUOBL9DUBCKI9DWSPEJA"}'  
    ```
    
    - Output:

    ```
    {
       "wasAddressSpentFrom": "true"
    }
       
    ```
     
     
### SweepSubscription

|Arg name             |type                 |Description                                                           |
|---------------------|---------------------|----------------------------------------------------------------------|
| newerThan           |timestamp            | all returned sweeps are newer than given timestamp,
|                     |                     | _timestamp_ _=_ _0_ means everything is retrieved



- Returns a list of detailed sweeps since a given timepoint


- curl example:
    - Call:
        
    ``` 
    curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "SweepSubscription"}' 
    ```
    
    - Output:
    ```
    {
        "event_0": {
            "bundleHash": "EWLCW9FFLSBUGZZOOLFQLTRJFKNGPUVCIOCQYTSDOSZLBCBJIIJZGPALGAKKANBTDYOBVQFOJHA9OVFOY",
            "timestamp": "1563796442000"
        },
        "event_1": {
            "bundleHash": "AJINYQCLKFYOCFWFLPESXAQGXYSZCHILJ9ZZCTNQOUGOFGTIOAXYZBCEWEXWDGAFFXBOXZJAPAUHVAZEC",
            "timestamp": "1567537268000"
        },
        "event_2": {
            "bundleHash": "GOHZXSDAFYDJTJ9GZKKCBAFFKDCTFGFIYDXADGUH9SJGFYPGIOWXEOJXOYSIGYANNWXEII9KSKUZZCHGX",
            "timestamp": "1567537470000"
        },
        "event_3": {
            "bundleHash": "N9TUFKNNTXITLOXXIHNGUULBKSIXDWHVZZZECPALRYZLZJF9LZZDNUTDJALZLJJPJ9TGTTNLFTGBUQWWW",
            "timestamp": "1569149768000",
            "withdrawalUuid": "50485062-f5f0-4cac-bad6-bb0362ae5138"
        }

        
        ...
    }


    ```
     
### SignBundle

- Returns a signature for a bundle created by the caller (external to hub)
  This command should be used to release the funds locked in an address that
  has already been swept (in case where user mistakenly funded an address more than once) 
  (Disabled by default, set _SignBundle_enabled_ to enable)

- Arguments

|Arg name             |type                 |Description                                                           |
|---------------------|---------------------|----------------------------------------------------------------------|
| address             |string (81/90 trytes)| The address whose signature is requested, must be an address returned
|                     |                     | by a previous call to `GetDepositAddress` 
| validateChecksum    |bool                 | Should set to true if _address_ 
|                     |                     | is 90 trytes long (81 + 9 checksum trytes)
| bundleHash          |string (81 trytes)   | The hash of the bundle to sign on
| authenticationToken |string               | A token used to authenticate the request
|                     |                     | (hub/docs/python/argon2_authentication_token.py)


- curl example:
    - Call:
        
     ```
     curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "SignBundle" , "address" : "SOMEADDRESS...", "validateChecksum": "false", "authenticationToken" : "SOMETOKEN"}' 
     ```
    
    - Output:
    ```
    {
        "signature": "THESIGNATURETRYTES"
    }

    ```
    
### RecoverFunds

- Creates a bundle that spends an already used address entirely into a provided address
  This command should be used to release the funds locked in an address that
  has already been swept (in case where user mistakenly funded an address more than once)
  It gives the caller less control than _SignBundle_ but it does not require extra work 
  (Disabled by default, set _RecoverFunds_enabled_ to enable)

- Arguments

|Arg name             |type                 |Description                                                           |
|---------------------|---------------------|----------------------------------------------------------------------|
| userId              |string               | The user's identifier
| address             |string (81 trytes)   | The address with the locked funds, must be a result of
|                     |                     | a previous call to `GetDepositAddress` 
| payoutAddress       |string (81/90 trytes)| The address to send the tokens found in _address_ to
|                     |                     |  
| validateChecksum    |bool                 | Should set to true if _payoutAddress_ 
|                     |                     | is 90 trytes long (81 + 9 checksum trytes)


- curl example:
    - Call:
        
    ``` 
    curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "RecoverFunds" , "userId" : "SomeUser", "address": "THEUSEDADDRESSTRYTES..", "payoutAddress": "LFABJNKAKJ...", "validateChecksum": "false"}' 
    ```
    
    - Output:
    ```
    {}
    ```
    
    
### ProcessTransferBatch

- Creates a list of balance records in the database

- Arguments

 A list of transfers with each element of the form:

|Arg name             |type                 |Description                                                           |
|---------------------|---------------------|----------------------------------------------------------------------|
| userId              |string               | The user's identifier
| amount              |integer              | The address with the locked funds, must be a result of



- curl example:
    - Call:
        
    ``` 
    curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "ProcessTransferBatch", "transfers": "[{userId: SomeUser;amount: -1},{userId: OtherUserID;amount: 1}]"}'     
    ```
    
    - Output:
    ```
    {}
    ```
    
### BalanceSubscription

- Returns a list of detailed balance events since a given timepoint

|Arg name             |type                 |Description                                                           |
|---------------------|---------------------|----------------------------------------------------------------------|
| newerThan           |timestamp            | all returned balance events are newer than given timestamp,
|                     |                     | _timestamp_ _=_ _0_ means everything is retrieved



- curl example:
    - Call:
        
    ``` 
    curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "BalanceSubscription"}' 
    ```
    
    - Output:
    ```
    {
        "event_0": {
            "type": "USER_ACCOUNT",
            "reason": "DEPOSIT",
            "userId": "SOMEUSER",
            "timestamp": "1563796442000",
            "sweepBundleHash": "EWLCW9FFLSBUGZZOOLFQLTRJFKNGPUVCIOCQYTSDOSZLBCBJIIJZGPALGAKKANBTDYOBVQFOJHA9OVFOY",
            "withdrawalUuid": ""
            "amount": "1000"
        },
        "event_1": {
            "type": "USER_ACCOUNT",
            "reason": "WITHDRAWAL",
            "userId": "SOMEUSER",
            "timestamp": "1563796562000",
            "sweepBundleHash": "",
            "withdrawalUuid": "4782e7d5-9ce4-477d-8fd0-32f5f3385db2"
            "amount": "-1000"
        },
        "event_2": {
            "type": "USER_ACCOUNT",
            "reason": "WITHDRAWAL_CANCELED",
            "userId": "SOMEUSER",
            "timestamp": "1563796604000",
            "sweepBundleHash": "",
            "withdrawalUuid": "4782e7d5-9ce4-477d-8fd0-32f5f3385db2"
            "amount": "1000"

        },

        ...
    }
    ```


    
    

     
             
             
 