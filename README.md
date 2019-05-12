# IOTA Hub
[![Build status](https://badge.buildkite.com/9c05b4a87f2242c78d62709136ca54a6d63fd48aa9b764f3e1.svg)](https://buildkite.com/iota-foundation/rpchub)

See [Getting Started](docs/getting_started.md) for information on how to use the Hub!

## Dependencies
- Modern GCC or Clang (or use a provided toolchain from `@iota_toolchains`)
- [bazel](https://github.com/bazelbuild/bazel/releases)
- pyparsing (`pip install pyparsing`)

## Supported database systems
- MariaDB. **DO NOT USE MySQL**. MySQL does not support CHECK constraints for data integrity. *We only test against MariaDB.*
  Please ensure that your MariaDB version is **>= 10.2.1** as this is the first version supporting [CHECK constraints](https://mariadb.com/kb/en/library/constraint/#check-constraints)

## Developing RPCHub
- Make sure that `buildifier` and `clang-format` are available on your `PATH`: (`go get -u github.com/bazelbuild/buildtools/buildifier`)
- Be sure to run `./hooks/autohook.sh install` after checkout!
- Pass `-c dbg` for building with debug symbols.

## How to build and run 
- see `docs/getting_started.md`

## Configuration
- If running on mainnet, set `-minWeightMagnitude` to at least 14
- You *MUST* pass a `-salt` parameter.
- Run your own IRI node and specify endpoint via `-apiAddress`

## Command Line Arguments
```
  Flags from common/flags/flags.h (used for both hub/signing_server binaries):
  
    -salt (Salt for local seed provider) type: string default: "" 
    -listenAddress (address to listen on) type: string default: "0.0.0.0:50051"
    -authMode ("credentials to use. can be {none, ssl}") type: string default: "none"
    -sslKey docs/ssl/server.key (path to SSL certificate key) type: string default: "/dev/null"
    -sslCert docs/ssl/server.crt (path to SSL certificate) type: string default: "/dev/null"
    -sslCA docs/ssl/ca.crt (Path to CA root) type: string default: "/dev/null"
    -maxConcurrentArgon2Hash (Max number of concurrent Argon2 Hash processes) type: int default: 4
    -argon2TCost (Time cost of Argon2) type: int default: 4
    -argon2MCost (Memory cost of Argon2 in bytes) type: int default: 1 << 17
    -argon2Parallelism (Number of threads to use in parallel for Argon2) type: int default: 1
    -argon2Mode (Argon2 mode to use: 1=argon2i;2,else=argon2id) type: int default: 2
    -keySecLevel (security level to use for IOTA signatures) type:int default: 2
    
  HUB -

  Flags from hub/db/db.cc:
    -db (Database name) type: string default: "hub"
    -dbDebug (Enable debug mode for database connection) type: bool
      default: false
    -dbHost (Database server host) type: string default: "127.0.0.1"
    -dbPassword (Database user password) type: string default: "password"
    -dbPort (Database server port) type: uint32 default: 3306
    -dbType (Type of DB) type: string default: "mariadb"
    -dbUser (Database user) type: string default: "user"

  Flags from hub/server/server.cc:
    -apiAddress (IRI node api to listen on. Format [host:port]) type: string
      default: "127.0.0.1:14265"
    -attachmentInterval (Attachment service check interval (0=disabled) [ms])
      type: uint32 default: 240000
    -authProvider (provider to use. can be {none, hmac}) type: string
      default: "none"
    -depth (Value for getTransacationToApprove depth parameter) type: uint32
      default: 3
    -hmacKeyPath (path to key used for HMAC encyption) type: string
      default: "/dev/null"
    -minWeightMagnitude (Minimum weight magnitude for POW) type: uint32
      default: 9
    -monitorInterval (Address monitor check interval (0=disabled) [ms])
      type: uint32 default: 60000
    -signingMode (crypto method to use {local,remote}) type: string
      default: "local"
    -signingProviderAddress (crypto provider address, should be provided if
      signingMode=remote) type: string default: "0.0.0.0:50052"
    -signingServerChainCert (Path to SSL certificate chain (server.crt))
      type: string default: "/dev/null"
    -signingServerKeyCert (Path to SSL certificate key (server.key))
      type: string default: "/dev/null"
    -signingServerSslCert (Path to SSL certificate (ca.cert)) type: string
      default: "/dev/null"
    -sweepInterval (Sweep interval (0=disabled) [ms]) type: uint32
      default: 600000
    -powMode (Proof of work mode, local or remote)
    -fetchTransactionMessages (Determines if Should store deposit transaction's message) 
      type: bool (--fetchTransactionMessages or --nofetchTransactionMessages)

  Flags from hub/service/sweep_service.cc:
    -sweep_max_deposit (Maximum number of user deposits to process per sweep.)
      type: uint32 default: 5
    -sweep_max_withdraw (Maximum number of withdraw requests to service per
      sweep.) type: uint32 default: 7
      
```

## Useful things
- Use `protofmt` from `go get -u -v github.com/emicklei/proto-contrib/cmd/protofmt` to format protobuf files
- Use `grpcc` to test: `grpcc -i -a localhost:50051 -p proto/hub.proto`
- Use `https://github.com/pseudomuto/protoc-gen-doc` to generate protobuf documentation
