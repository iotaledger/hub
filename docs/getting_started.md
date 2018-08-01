# IOTA Hub getting started.

## Before we start:

- Hub is not an IOTA node, it is a tool for managing transfers from/to addresses in a simple and secure manner

- User should send tokens to a deposit address only once, since deposit addresses are swept and aren't reusable

- In case user sent tokens to a deposit address that has been swept, we offer a mechanism to recover the funds (see docs/hip/001-get_address_secret.md)

- To facilitate toolchain / build environment setup we have provided toolchain configurations to build the Hub against. To use these, see the [toolchains repository](https://github.com/iotaledger/toolchains) for more details. As an example, to use the provided `x86-64-core-i7` toolchain, you'd build the signing server like this: `bazel build -c opt --crosstool_top='@iota_toolchains//tools/x86-64-core-i7--glibc--bleeding-edge-2018.07-1:toolchain' --cpu=x86_64 --host_crosstool_top=@bazel_tools//tools/cpp:toolchain //signing_server/...`

## Building the hub

- bazel run -c opt //hub:hub -- <parameters>
- bazel build //hub:hub

## Building signing_server (for remote signing_server)

- bazel run -c opt //signing_server:signing_server -- <parameters>
- bazel build //signing_server:signing_server

## Setup the database (this example is using MariaDB)

- Install mariadb
- run mariadb CLI:
    - mysql -h127.0.0.1 --user root -ppassword
    - MariaDB [(none)]> create database db_name;
    - MariaDB [(none)]> use db_name;
    - MariaDB [(none)]> source schema/schema.sql;
    - MariaDB [(none)]> source schema/triggers.mariadb.sql;

## Runnig the hub

- ./bazel-bin/hub/hub --salt yoursaltcharachters --apiAddress your_iri_node_uri --db db_name --dbUser your_user --dbPassword user_password --monitorInterval xxx --attachmentInterval yyy --sweepInterval zzz
- For detailed explanation about program arguments, see README
- For running the hub with remote salt provider (--signingMode=remote) run the signing_provider first

## Running the signing_server
- ./bazel-bin/signing_server/signing_server --salt abcdefghijklmnopqrstuvwxyz --authMode=ssl  -sslKey docs/ssl/server.key -sslCert docs/ssl/server.crt -sslCA docs/ssl/ca.crt --listenAddress=localhost:50052

### Run via Docker
You can build a docker image for `RPCHub` via `bazel build -c opt //docker:hub`

## Calling client commands

- The Hub is a GRPC server (https://grpc.io/docs/tutorials/)
- Methods and arguments are available under proto/hub.proto (methods) and proto/messages.proto (request/response types)
