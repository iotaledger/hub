# IOTA Hub
## Dependencies
- Modern GCC or Clang
- [bazel](https://github.com/bazelbuild/bazel/releases)
- sqlite3
- system-wide installation of mariadb-connector-c 
- pyparsing (pip install pyparsing)
- buildifier: (`go get -u github.com/bazelbuild/buildtools/buildifier`)

## Supported database systems
- MariaDB. Note that we *strongly discourage* the use of *MySQL* because it does not support CHECK constraints for data integrity. We only test against MariaDB.
  Please ensure that your MariaDB version is *>= 10.2.1* as this is the first version supporting [CHECK constraints](https://mariadb.com/kb/en/library/constraint/#check-constraints)

## Developing RPCHub
- Be sure to run `./hooks/autohook.sh install` after checkout!
- Pass `-c dbg` for building with debug symbols.

## How to run
- `bazel run -c opt //hub:hub -- <parameters>`
- `bazel build //hub:hub`
- `rm hub.db; sqlite3 hub.db < schema/schema.sql; GLOG_logtostderr=true GLOG_v=3 ./bazel-bin/hub/hub --salt abcdefghijklmnopqrstuvwxyz --apiAddress localhost:14700`

### Run via Docker
You can build a docker image for `RPCHub` via `bazel build -c opt //docker:hub`

## Configuration
- If running on mainnet, set `-minWeightMagnitude` to at least 14
- You *MUST* pass a `-salt` parameter.
- Run your own IRI node and specify endpoint via `-apiAddress`

## Command Line Arguments
```
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
    -attachmentInterval (Attachment service check interval [ms]) type: uint32
      default: 240000
    -authMode (credentials to use. can be {none}) type: string default: "none"
    -depth (Value for getTransacationToApprove depth parameter) type: uint32
      default: 9
    -listenAddress (address to listen on) type: string default: "0.0.0.0:50051"
    -minWeightMagnitude (Minimum weight magnitude for POW) type: uint32
      default: 9
    -monitorInterval (Address monitor check interval [ms]) type: uint32
      default: 60000
    -salt (Salt for local seed provider) type: string default: ""
    -sweepInterval (Sweep interval [ms]) type: uint32 default: 600000

  Flags from hub/service/sweep_service.cc:
    -sweep_max_deposit (Maximum number of user deposits to process per sweep.)
      type: uint32 default: 5
    -sweep_max_withdraw (Maximum number of withdraw requests to service per
      sweep.) type: uint32 default: 7
```

## Useful things
- Use `clang-format`
- Use `protofmt` from `go get -u -v github.com/emicklei/proto-contrib/cmd/protofmt` to format
- Use `grpcc` to test: `grpcc -i -a localhost:50051 -p proto/hub.proto`
- Use `https://github.com/pseudomuto/protoc-gen-doc` to generate protobuf documentation
