# IOTA Hub getting started.

## Before we start:

- Hub is not an IOTA node, it is a tool for managing transfers from/to addresses in a simple and secure manner

- User should send tokens to a deposit address only once, since deposit addresses are swept and aren't reusable

- In case user sent tokens to a deposit address that has been swept, we offer a mechanism to recover the funds (see docs/hip/001-get_address_secret.md)

## Building

- see README file

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
- For runing the hub with remote salt provider (--signingMode=remote) run the signing_provider first

## Runing the signing_server
- ./bazel-bin/signing_server/signing_server --salt abcdefghijklmnopqrstuvwxyz --authMode=ssl  -sslKey docs/ssl/server.key -sslCert docs/ssl/server.crt -sslCA docs/ssl/ca.crt --listenAddress=localhost:50052

### Run via Docker
You can build a docker image for `RPCHub` via `bazel build -c opt //docker:hub`

## Calling client commands

- The Hub is a GRPC server (https://grpc.io/docs/tutorials/)

- Methods and arguments are available under proto/hub.proto (methods) and proto/messages.proto (request/response types)
