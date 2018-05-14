workspace(name = "org_iota_hub")

# EXTERNAL RULES
git_repository(
    name = "org_pubref_rules_protobuf",
    remote = "https://github.com/pubref/rules_protobuf",
    tag = "v0.8.2",
)

http_archive(
    name = "io_bazel_rules_go",
    sha256 = "f70c35a8c779bb92f7521ecb5a1c6604e9c3edd431e50b6376d7497abc8ad3c1",
    url =
        "https://github.com/bazelbuild/rules_go/releases/download/0.11.0/rules_go-0.11.0.tar.gz",
)

git_repository(
    name = "com_github_nelhage_rules_boost",
    commit = "69e96e724cbe746f63d993fa8f6bf772e1ebc5cc",
    remote = "https://github.com/nelhage/rules_boost",
)

# DEPENDENCIES
git_repository(
    name = "io_bazel_rules_docker",
    remote = "https://github.com/bazelbuild/rules_docker.git",
    tag = "v0.4.0",
)

git_repository(
    name = "com_github_gflags_gflags",
    commit = "e292e0452fcfd5a8ae055b59052fc041cbab4abf",
    remote = "https://github.com/gflags/gflags.git",
)

git_repository(
    name = "com_github_google_glog",
    commit = "2f493d292c92abf16ebd46cfd0cc0bf8eef5724d",
    remote = "https://github.com/google/glog.git",
)

git_repository(
    name = "com_google_googletest",
    commit = "a6f06bf2fd3b832822cd4e9e554b7d47f32ec084",
    remote = "https://github.com/google/googletest.git",
)

new_git_repository(
    name = "hinnant_date",
    build_file = "BUILD.date",
    commit = "e5c69d84ab5db3e06170b8eedec1d87841c7fb22",
    remote = "https://github.com/HowardHinnant/date.git",
)

new_git_repository(
    name = "sqlpp11",
    build_file = "BUILD.sqlpp11",
    commit = "9d64a5c7411f84cad1da71b23118634b5a62a063",
    remote = "https://github.com/rbock/sqlpp11.git",
)

new_git_repository(
    name = "sqlpp11sqlite",
    build_file = "BUILD.sqlpp11sqlite",
    commit = "cf37829fc9828a36afa050f960d09abcaf6aeb6a",
    remote = "https://github.com/rbock/sqlpp11-connector-sqlite3.git",
)

new_git_repository(
    name = "sqlpp11mysql",
    build_file = "BUILD.sqlpp11mysql",
    commit = "8a48bc2d6157fc445cda5b9beac9b7901fff625c",
    remote = "https://github.com/rbock/sqlpp11-connector-mysql.git",
)

new_http_archive(
    name = "argon2",
    build_file = "BUILD.argon2",
    sha256 = "eaea0172c1f4ee4550d1b6c9ce01aab8d1ab66b4207776aa67991eb5872fdcd8",
    strip_prefix = "phc-winner-argon2-20171227",
    url = "https://github.com/P-H-C/phc-winner-argon2/archive/20171227.tar.gz",
)

new_http_archive(
    name = "sqlite3",
    build_file = "BUILD.sqlite3",
    sha256 = "4239a1f69e5721d07d9a374eb84d594225229e54be4ee628da2995f4315d8dfc",
    strip_prefix = "sqlite-amalgamation-3230100",
    url = "https://www.sqlite.org/2018/sqlite-amalgamation-3230100.zip",
)

git_repository(
    name = "rules_iota",
    commit = "5622593910361262b248ad165aaf60bc87d0fa16",
    remote = "https://gitlab.com/iota-foundation/software/rules_iota.git",
)

http_archive(
    name = "org_iota_entangled",
    strip_prefix = "entangled-develop",
    url =
        "https://gitlab.com/iota-foundation/software/entangled/-/archive/develop/entangled-develop.tar.gz",
)

new_git_repository(
    name = "nlohmann",
    build_file = "BUILD.nlohmann_json",
    commit = "435fc32a697188a43c86c559e60069711317ffc9",
    remote = "https://github.com/th0br0/json.git",
)

new_http_archive(
    name = "keccak",
    build_file = "@rules_iota//:build/BUILD.keccak",
    sha256 = "35c63620721ac4da418d4bb427ba7ae9aae76b4a1bea7758d6694a29f6e6488a",
    strip_prefix = "KeccakCodePackage-c737139c81fd191699886a9a74d3220f6e300b29",
    url =
        "https://github.com/gvanas/KeccakCodePackage/archive/c737139c81fd191699886a9a74d3220f6e300b29.zip",
)

new_http_archive(
    name = "mariadb_connector",
    build_file = "BUILD.mariadb_connector",
    sha256 = "6eff680cd429fdb32940f6ea4755a997dda1bb00f142f439071f752fd0b200cf",
    strip_prefix = "mariadb-connector-c-3.0.4-src",
    url = "http://ftp.hosteurope.de/mirror/mariadb.org//connector-c-3.0.4/mariadb-connector-c-3.0.4-src.tar.gz",
)

load(
    "@io_bazel_rules_docker//container:container.bzl",
    "container_pull",
    container_repositories = "repositories",
)
load(
    "@io_bazel_rules_docker//cc:image.bzl",
    _cc_image_repos = "repositories",
)
load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
load("@org_pubref_rules_protobuf//cpp:rules.bzl", "cpp_proto_repositories")
load(
    "@io_bazel_rules_go//go:def.bzl",
    "go_rules_dependencies",
    "go_register_toolchains",
    "go_prefix",
)
load("@org_pubref_rules_protobuf//go:rules.bzl", "go_proto_repositories")
load(
    "@org_pubref_rules_protobuf//grpc_gateway:rules.bzl",
    "grpc_gateway_proto_repositories",
)

_cc_image_repos()

boost_deps()

cpp_proto_repositories()

go_rules_dependencies()

go_register_toolchains()

go_proto_repositories()

grpc_gateway_proto_repositories()
