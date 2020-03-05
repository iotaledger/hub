<h1 align="center">
  <br>
  <a href="https://docs.iota.org/docs/wallets/0.1/hub/introduction/overview"><img src="hub.png"></a>
</h1>

<h2 align="center">A wallet management system for cryptocurrency exchanges and custodians</h2>

<p align="center">
    <a href="https://docs.iota.org/docs/wallets/0.1/hub/introduction/overview" style="text-decoration:none;">
    <img src="https://img.shields.io/badge/Documentation%20portal-blue.svg?style=for-the-badge" alt="Developer documentation portal">
</p>
<p align="center">
  <a href="https://discord.iota.org/" style="text-decoration:none;"><img src="https://img.shields.io/badge/Discord-9cf.svg?logo=discord" alt="Discord"></a>
    <a href="https://iota.stackexchange.com/" style="text-decoration:none;"><img src="https://img.shields.io/badge/StackExchange-9cf.svg?logo=stackexchange" alt="StackExchange"></a>
    <a href="https://raw.githubusercontent.com/iotaledger/hub/master/LICENSE" style="text-decoration:none;"><img src="https://img.shields.io/github/license/iotaledger/hub.svg" alt="Apache 2.0 license"></a>
    <a href="https://buildkite.com/iota-foundation/hub" style="text-decoration:none;"><img src="https://badge.buildkite.com/9c05b4a87f2242c78d62709136ca54a6d63fd48aa9b764f3e1.svg" alt="Build status"></a>
</p>

<p align="center">
  <a href="#about">About</a> ◈
  <a href="#prerequisites">Prerequisites</a> ◈
  <a href="#installation">Installation</a> ◈
  <a href="#getting-started">Getting started</a> ◈
  <a href="#api-reference">API reference</a> ◈
  <a href="#supporting-the-project">Supporting the project</a> ◈
  <a href="#joining-the-discussion">Joining the discussion</a> 
</p>

---

## About

Hub is a wallet management system for cryptocurrency exchanges and custodians. Through its application programming interfaces (APIs), Hub offers you an easy way to integrate IOTA into your application by allowing you to do the following:

- Create user accounts
- Manage users' [IOTA tokens](root://getting-started/0.1/clients/token.md)
- Keep a record of users' [addresses](root://getting-started/0.1/clients/addresses.md)
- Keep a record of users' [transactions](root://getting-started/0.1/transactions/transactions.md)

This is beta software, so there may be performance and stability issues.
Please report any issues in our [issue tracker](https://github.com/iotaledger/hub/issues/new).

## Prerequisites

To install Hub, you need one of the following:

- Docker
- A Linux operating system

## Installation

For installation instructions, see the [documentation portal](https://docs.iota.org/docs/wallets/0.1/hub/how-to-guides/install-hub).

## Getting started

Depending on how you configured Hub, you can use either its gRPC or REST API to start creating new users.
For a guide on using the gRPC API, see the [documentation portal](https://docs.iota.org/docs/wallets/0.1/hub/how-to-guides/get-started-with-grpc-api).

### Getting started with the RESTful API

To create a new user, do the following:

```bash
curl http://localhost:50051 \
-X POST \
-H 'Content-Type: application/json' \
-H 'X-IOTA-API-Version: 1' \
-d '{
  "command": "CreateUser",
  "userId": "Jake"
}'
```
To generate a new deposit address for the user, do the following:

```bash
curl http://localhost:50051 \
-X POST \
-H 'Content-Type: application/json' \
-H 'X-IOTA-API-Version: 1' \
-d '{
  "command": "GetDepositAddress",
  "userId": "Jake",
  "includeChecksum": "true"
}'
```
In the output, you should see a 90-tryte deposit address:

```
"address": "RDZVDZKRBX9T9L9XXONXDVJDRKYPAABWMQLORGCDCWHDDTSOPRZPCQB9AIZZWZAQ9NBZNVUUUSPQHRGWDYZUVP9WSC"
```

**Note:** In the database, addresses are always saved without the checksum.

## API reference

For details on all available API methods, see the [documentation portal](https://docs.iota.works/docs/wallets/0.1/hub/references/restful-api-reference).

## Supporting the project

If you want to contribute to Hub, consider posting a [bug report](https://github.com/iotaledger/hub/issues/new), [feature request](https://github.com/iotaledger/hub/issues/new) or a [pull request](https://github.com/iotaledger/hub/pulls/).

See the [contributing guidelines](CONTRIBUTING.md) for more information.

## Joining the discussion

If you want to get involved in the community, need help with getting setup, have any issues related with the library or just want to discuss IOTA, Distributed Registry Technology (DRT) and IoT with other people, feel free to join our [Discord](https://discord.iota.org/).
