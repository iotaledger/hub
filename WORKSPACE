workspace(name = "org_iota_hub")

# EXTERNAL RULES
git_repository(
    name = "org_pubref_rules_protobuf",
    commit = "0c59d9145c0d3bfba2a61a04392a950b088a9b83",
    remote = "https://github.com/pubref/rules_protobuf",
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
    build_file = "third-party/date/BUILD",
    commit = "e5c69d84ab5db3e06170b8eedec1d87841c7fb22",
    remote = "https://github.com/HowardHinnant/date.git",
)

new_git_repository(
    name = "sqlpp11",
    build_file = "third-party/sqlpp11/BUILD",
    commit = "9d64a5c7411f84cad1da71b23118634b5a62a063",
    remote = "https://github.com/rbock/sqlpp11.git",
)

new_git_repository(
    name = "sqlpp11sqlite",
    build_file = "third-party/sqlpp11sqlite/BUILD",
    commit = "cf37829fc9828a36afa050f960d09abcaf6aeb6a",
    remote = "https://github.com/rbock/sqlpp11-connector-sqlite3.git",
)

new_git_repository(
    name = "sqlpp11mysql",
    build_file = "third-party/sqlpp11mysql/BUILD",
    commit = "8a48bc2d6157fc445cda5b9beac9b7901fff625c",
    remote = "https://github.com/rbock/sqlpp11-connector-mysql.git",
)

new_http_archive(
    name = "argon2",
    build_file = "third-party/argon2/BUILD",
    sha256 = "eaea0172c1f4ee4550d1b6c9ce01aab8d1ab66b4207776aa67991eb5872fdcd8",
    strip_prefix = "phc-winner-argon2-20171227",
    url = "https://github.com/P-H-C/phc-winner-argon2/archive/20171227.tar.gz",
)

new_http_archive(
    name = "sqlite3",
    build_file = "third-party/sqlite3/BUILD",
    sha256 = "4239a1f69e5721d07d9a374eb84d594225229e54be4ee628da2995f4315d8dfc",
    strip_prefix = "sqlite-amalgamation-3230100",
    url = "https://www.sqlite.org/2018/sqlite-amalgamation-3230100.zip",
)

git_repository(
    name = "rules_iota",
    commit = "941f32af098a6976dfdb137550da6b58811d4432",
    remote = "https://github.com/iotaledger/rules_iota.git",
)

http_archive(
    name = "org_iota_entangled",
    strip_prefix = "entangled-develop",
    url =
        "https://gitlab.com/iota-foundation/software/entangled/-/archive/develop/entangled-develop.tar.gz",
)

new_git_repository(
    name = "iota_lib_cpp",
    build_file = "third-party/iota_lib_cpp/BUILD",
    commit = "9971c832e6a38972803a4d1506a78c36451c3df3",
    remote = "https://github.com/th0br0/iota.lib.cpp.git",
)

new_git_repository(
    name = "mariadb_connector",
    build_file = "third-party/mariadb_connector/BUILD",
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

_cc_image_repos()

iota_deps()

cpp_proto_repositories()

go_rules_dependencies()

go_register_toolchains()

go_proto_repositories()

grpc_gateway_proto_repositories()
