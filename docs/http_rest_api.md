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
        
        ```curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "CreateUser", "userId": "SomeUser"}' ```
    
    - Output:
    ```{}```
    
    
### GetDepositAddress

- Get's a random address for user to deposit funds into (will be monitored by Hub monitoring_service)

- Arguments

|Arg name    |type          |Description                    |
|------------|--------------|-------------------------------|
| userId     |string        | unique user id                |

- curl example:
    - Call:
        
        ```curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "GetDepositAddress", "userId": "SomeUser"}' ```
    
    - Output:
    ``` {"address": "DILTSLPCVZTLRICPK9PZPAZOTKVUSAAYPOYHMLIIQKPASAGXHGAEZPGGEQGQZCKAHNKPTNIXJPFC9LOCZ"} ```
 

### GetBalance

- Get's the balance for a user

- Arguments

|Arg name    |type          |Description                    |
|------------|--------------|-------------------------------|
| userId     |string        | unique user id                |

- curl example:
    - Call:
        
        ``` curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "GetBalance", "userId": "SomeUser"}' ```
    
    - Output:
    ````{"available": "1000"}````
    
 
### GetAddressInfo

- Get's the userId for a given address

- Arguments

|Arg name    |type          |Description                    |
|------------|--------------|-------------------------------|
| address    |string        | The address in question       |

- curl example:
    - Call:
        
        ``` curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "GetAddressInfo", "address": "DILTSLPCVZTLRICPK9PZPAZOTKVUSAAYPOYHMLIIQKPASAGXHGAEZPGGEQGQZCKAHNKPTNIXJPFC9LOCZ"}' ```
    
    - Output:
    ``` {"userId": "SomeUSer"} ```
    
    
    
### GetUserHistory

- Returns a list of all user balance events

- Arguments

|Arg name    |type          |Description                    |
|------------|--------------|-------------------------------|
| userId     |string        | unique user id                |

- curl example:
    - Call:
        
        ``` curl  http://localhost:50051  -X POST   -H 'Content-Type: application/json'   -H 'X-IOTA-API-Version:  1'   -d '{"command": "GetUserHistory", "userId": "SomeUser"}' ```
    
    - Output:
    ````{
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
            "sweepBundleHash": "4782e7d5-9ce4-477d-8fd0-32f5f3385db2",
            "withdrawalUUID": ""
        },
        "event_2": {
            "userID": "SomeUser",
            "timestamp": "1563796604000",
            "amount": "1",
            "reason": "WITHDRAWAL_CANCELED",
            "sweepBundleHash": "4782e7d5-9ce4-477d-8fd0-32f5f3385db2",
            "withdrawalUUID": ""
        }
    }````
    
    The list of possible user balance event "reason" are under "hub/commands/converter.cc"
    
 