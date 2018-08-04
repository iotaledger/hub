workspace(name = "org_iota_hub")

# EXTERNAL RULES
git_repository(
    name = "org_pubref_rules_protobuf",
    commit = "0c59d9145c0d3bfba2a61a04392a950b088a9b83",
    remote = "https://github.com/pubref/rules_protobuf",
)

git_repository(
    name = "iota_toolchains",
    commit = "6b501df8e7f3bc3b143c894737fbb1d82e914762",
    remote = "https://github.com/iotaledger/toolchains.git",
)

http_archive(
    name = "io_bazel_rules_go",
    sha256 = "f70c35a8c779bb92f7521ecb5a1c6604e9c3edd431e50b6376d7497abc8ad3c1",
    url =
        "https://github.com/bazelbuild/rules_go/releases/download/0.11.0/rules_go-0.11.0.tar.gz",
)

git_repository(
    name = "bazel_skylib",
    remote = "https://github.com/bazelbuild/bazel-skylib.git",
    tag = "0.4.0",
)

git_repository(
    name = "build_bazel_rules_apple",
    remote = "https://github.com/bazelbuild/rules_apple.git",
    tag = "0.5.0",
)

# DEPENDENCIES
git_repository(
    name = "io_bazel_rules_docker",
    remote = "https://github.com/bazelbuild/rules_docker.git",
    tag = "v0.4.0",
)

new_git_repository(
    name = "hinnant_date",
    build_file = "third-party/date/BUILD.bzl",
    commit = "e5c69d84ab5db3e06170b8eedec1d87841c7fb22",
    remote = "https://github.com/HowardHinnant/date.git",
)

new_git_repository(
    name = "sqlpp11",
    build_file = "third-party/sqlpp11/BUILD.bzl",
    commit = "ba05135d47e8674f429ba1e03a0259a48f05a4c4",
    remote = "https://github.com/rbock/sqlpp11.git",
)

new_git_repository(
    name = "sqlpp11sqlite",
    build_file = "third-party/sqlpp11sqlite/BUILD.bzl",
    commit = "cf37829fc9828a36afa050f960d09abcaf6aeb6a",
    remote = "https://github.com/rbock/sqlpp11-connector-sqlite3.git",
)

new_git_repository(
    name = "sqlpp11mysql",
    build_file = "third-party/sqlpp11mysql/BUILD.bzl",
    commit = "8a48bc2d6157fc445cda5b9beac9b7901fff625c",
    remote = "https://github.com/rbock/sqlpp11-connector-mysql.git",
)

git_repository(
    name = "rules_iota",
    commit = "0d0681a6dc4069a1bd2c7576e6430b2b04f937ad",
    remote = "https://github.com/iotaledger/rules_iota.git",
)

http_archive(
    name = "org_iota_entangled",
    sha256 = "f35728f50c1e44abbdfe3cb3fa3567dbe078e9fdd19c022b6e442589dd511b68",
    strip_prefix = "entangled-fb14f6d710195bb33caf8d72d3c686d47133fb71",
    url = "https://github.com/iotaledger/entangled/archive/fb14f6d710195bb33caf8d72d3c686d47133fb71.zip",
)

new_git_repository(
    name = "iota_lib_cpp",
    build_file = "third-party/iota_lib_cpp/BUILD.bzl",
    commit = "9971c832e6a38972803a4d1506a78c36451c3df3",
    remote = "https://github.com/th0br0/iota.lib.cpp.git",
)

new_git_repository(
    name = "mariadb_connector",
    build_file = "third-party/mariadb_connector/BUILD.bzl",
    commit = "184a16d2f1d0bb24bea6bcf23e1604093fef8f93",
    remote = "https://github.com/MariaDB/mariadb-connector-c.git",
)

load("@rules_iota//:defs.bzl", "iota_deps")
load(
    "@io_bazel_rules_docker//container:container.bzl",
    "container_pull",
    container_repositories = "repositories",
)
load(
    "@io_bazel_rules_docker//cc:image.bzl",
    _cc_image_repos = "repositories",
)
load("@org_pubref_rules_protobuf//cpp:rules.bzl", "cpp_proto_repositories")
load(
    "@io_bazel_rules_go//go:def.bzl",
    "go_prefix",
    "go_register_toolchains",
    "go_rules_dependencies",
)
load("@org_pubref_rules_protobuf//go:rules.bzl", "go_proto_repositories")
load(
    "@org_pubref_rules_protobuf//grpc_gateway:rules.bzl",
    "grpc_gateway_proto_repositories",
)
load("@iota_toolchains//:toolchains.bzl", "setup_toolchains")

setup_toolchains()

iota_deps()

_cc_image_repos()

cpp_proto_repositories()

go_rules_dependencies()

go_register_toolchains()

go_proto_repositories()

grpc_gateway_proto_repositories()
