# HIP-002: Signing server

## Problem description 
By knowing a UUID and the salt anyone can generate a seed and derive an address/signature easily.
If hub uses a **local** signature provider, then there is a single point of failure, since all UUIDs
and the salt are in the same place, making it easy to generate signatures and steal all funds associated
with hub's non-empty deposit addresses

## Proposed solution
Creating a separate remote signature provider (*signing_server*) that will expose endpoints 
for generating addresses/signatures and will be the only one to hold the salt, the new server will expose its 
endpoints to a client app (hub) only when certified via SSL

## Motivation
### Separation of responsibilities
If the Hub is compromised, there's now no way to spend any of the funds in any of the addresses, 
the attacker will need to be serviced by the *signing_server* and that will require him/her to have the SSL
certificate components that *signing_server* is using, if by any chance the attacker is able to
control hub, then as soon that has been noticed, *signing_server* should be stopped and hub user
should start a recovery process.
If Salt is being compromised, there's no way to spend funds, since attackers needs to know the UUIDs that
were used to generate an address in order to generate a seed and then a signature,
Nevertheless, the user should stop generating new addresses with the old salt, move all funds to safe 
address/addresses and start the *signing_server* with new salt and a new SSL certificate 

## Questions
1. How should a recovery process be defined after hub is compromised
2. How should a recovery process be defined after signing_server is compromised

Service:
```proto
service SigningServer {
    // Gets the address for the UUID
    RPC GetAddressForUUID (GetAddressForUUIDRequest) returns (GetAddressForUUIDReply);
    // Gets the signature for the UUID
    RPC GetSignatureForUUID (GetSignatureForUUIDRequest) returns (GetSignatureForUUIDReply);
    // Gets the security level of the provider
    RPC GetSecurityLevel (GetSecurityLevelRequest) returns (GetSecurityLevelReply);
}
```

Messages:
```
enum ErrorCode {
    // Unused.
    EC_UNKNOWN = 0;
}

/*
 * The generic Error that will be used to return an error code on, e.g. Status::CANCELLED.
 *
 * The error will be serialised and stored in the Status' detail field.
 */
message Error {
  ErrorCode code = 1;
}

/*
 * Request for getting address
 */
message GetAddressForUUIDRequest {
    string UUID = 1;
}

/*
 * Reply for getting address
 */
message GetAddressForUUIDReply {
    string address = 1;
}

/*
 * Request for getting signature
 */
message GetSignatureForUUIDRequest {
    string UUID = 1;
    string bundleHash = 2;
}

/*
 * Reply for getting signature
 */
message GetSignatureForUUIDReply {
    string signature = 1;
}

/*
 * Request for getting security level
 */
message GetSecurityLevelRequest {
    string UUID = 1;
}
/*
 * Reply for getting security level
 */
message GetSecurityLevelReply {
    uint32 securityLevel = 1;
}
```
**hub**:

```c++
class RemoteSigningProvider : public common::crypto::CryptoProviderBase {...}
```

The above class will implement base methods to generate addresses/signatures
and will internally issue RPC calls to the *signing_server*

**signing_server**
```c++
class SigningServerImpl final : public SigningServer::Service {
   public:
    /// Constructor
    SigningServerImpl() {}
    /// Destructor
    ~SigningServerImpl() override {}
  
    // Gets the address for the UUID
    gRPC::Status GetAddressForUUID(::gRPC::ServerContext* context,
                                   const GetAddressForUUIDRequest* request,
                                   GetAddressForUUIDReply* response) override;
    // Gets the signature for the UUID
    gRPC::Status GetSignatureForUUID(::gRPC::ServerContext* context,
                                     const GetSignatureForUUIDRequest* request,
                                     GetSignatureForUUIDReply* response) override;
    // Gets the security level of the provider
    gRPC::Status GetSecurityLevel(::gRPC::ServerContext* context,
                                  const GetSecurityLevelRequest* request,
                                  GetSecurityLevelReply* response) override;
  };
  ```


