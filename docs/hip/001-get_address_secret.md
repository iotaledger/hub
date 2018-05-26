# HIP-001: GetAddressSecret

## Problem description 
Even though exchanges warn against this explicitly, might end up sending tokens to addresses that have already been used as inputs for a sweep.
Due to address security reasons, the Hub does not monitor such addresses any longer.
However, when large amounts of funds are at risk, users might sue exchanges / go to the press / similar. To prevent this from happening, an exchange must have
a means to access the funds (i.e. send back to the user).

## Proposed solution
Offer the following gRPC method call that can be enabled via a runtime flag but is *disabled by default*.

```proto
// add to ErrorCode:
enum ErrorCode {
  // Address is unknown or has not been used as input for a sweep
  ADDRESS_NOT_ELIGIBLE;
  // The authentication token could not be verified.
  INVALID_AUTHENTICATION;
}

message GetAddressSecretRequest {
  // The Hub-owned IOTA address that should be accessed.
  string address = 1;
  // Authentication payload 
  string authentication = 2;
}

message GetAddressSecretResponse {
  // The address' seed
  string seed = 1;
}

service Hub {
  // Reveals the seed for a given user address.
  rpc GetAddressSecret(GetAddressSecretRequest) returns GetAddressSecretResponse);
}
```

Internally, the call will:
1. Check that address was used in sweep before.
2. Verify authentication token. 
3. Mark (address, authentication token) as used.
3. Calculate & return seed

If an exchange wants to provide a custom audit system integration, they'll need to implement the following interface and register it at startup in their local fork.

```c++
// @file hub/auth/provider.h

namespace hub {
namespace auth {

enum AuthContext {
  GET_ADDRESS_SECRET = 0;
};

class AuthProvider {
public:
  virtual bool validateToken(const std::string& token) = 0;
};

}
}

```

## Motivation
### Authentication payload
Some exchanges might have internal safeguards to prevent rogue internal actors from accessing secrets. Therefore, the Hub needs to be able to integrate with these by exposing an interface they can program against.


## Questions
1. Should the call be able to be called multiple times? 
2. Which authentication method should be provided by default?
   1. Password
   2. HMAC


