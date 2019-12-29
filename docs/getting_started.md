# IOTA Hub: Getting started.

## Before we start:
- Hub is not an IOTA node, it is a tool for managing transfers from/to addresses in a simple and secure manner
- User should send tokens to a deposit address only once, since deposit addresses are swept and aren't reusable
- In case user sent tokens to a deposit address that has been swept, we offer a mechanism to recover the funds (see docs/hip/001-sign_bundle.md)
- To facilitate toolchain / build environment setup we have provided toolchain configurations to build the Hub against. 
  To use these, see the [toolchains repository](https://github.com/iotaledger/toolchains) for more details. 
  As an example, to use the provided `x86-64-core-i7` toolchain, you'd build the signing server like this: 
  `bazel build -c opt --crosstool_top='@iota_toolchains//tools/x86-64-core-i7--glibc--bleeding-edge-2018.07-1:toolchain' --cpu=x86_64 --host_crosstool_top=@bazel_tools//tools/cpp:toolchain //signing_server/...`

## Building the hub
- `bazel build -c opt //hub:hub`

## Building signing_server (for remote signing_server)
- `bazel build -c opt //signing_server`

## Setup the database (this example is using MariaDB)
- Install `MariaDB` (e.g. via Docker)
- Import schemas:
  - `mysql -h127.0.0.1 --user root -ppassword < schema/schema.sql`
  - `mysql -h127.0.0.1 --user root -ppassword < schema/triggers.mariadb.sql`

## Running the hub
- `./bazel-bin/hub/hub --salt yoursaltcharachters --apiAddress your_iri_node_uri --db db_name --dbUser your_user --dbPassword user_password --monitorInterval xxx --attachmentInterval yyy --sweepInterval zzz`
- For detailed explanation about program arguments, see `README.md` or start with `--help`
- For running the hub with remote salt provider (`--signingMode=remote`) run the `signing_provider` first

## Running the signing_server
- `./bazel-bin/signing_server/signing_server --salt abcdefghijklmnopqrstuvwxyz --authMode=ssl  -sslKey docs/ssl/server.key -sslCert docs/ssl/server.crt -sslCA docs/ssl/ca.crt --listenAddress=localhost:50052`

### Run via Docker
You can build and run a docker image for `RPCHub` via `bazel run -c opt //docker:hub`

## Calling client commands
- The Hub exposes both gRPC (https://grpc.io/docs/tutorials/) and REST apis
- GRPC: Methods and arguments are available under proto/hub.proto (methods) and proto/messages.proto (request/response types)
- REST: [HTTP_REST_API](http_rest_api.md)
