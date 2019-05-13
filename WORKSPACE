workspace(name = "org_iota_hub")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# EXTERNAL RULES
http_archive(
    name = "com_github_grpc_grpc",
    strip_prefix = "grpc-1.17.2",
    urls = [
        "https://github.com/grpc/grpc/archive/v1.17.2.tar.gz",
    ],
)

git_repository(
    name = "io_bazel_rules_python",
    commit = "f3a6a8d00a51a1f0e6d61bc7065c19fea2b3dd7a",
    remote = "https://github.com/bazelbuild/rules_python.git",
)

git_repository(
    name = "rules_iota",
    commit = "b15744b9ea520717752c866d5afc769c3b6b68f3",
    remote = "https://github.com/iotaledger/rules_iota.git",
)

git_repository(
    name = "iota_toolchains",
    commit = "9a6c200013d971524f8427b6a404bab0a67a9328",
    remote = "https://github.com/iotaledger/toolchains.git",
)

git_repository(
    name = "bazel_toolchains",
    commit = "b6875a7bb09b4fa1db8ea347852c0dc9ccae74ab",
    remote = "https://github.com/th0br0/bazel-toolchains.git",
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

# DEPENDENCIES
http_archive(
    name = "com_google_protobuf",
    strip_prefix = "protobuf-66dc42d891a4fc8e9190c524fd67961688a37bbe",
    url = "https://github.com/google/protobuf/archive/66dc42d891a4fc8e9190c524fd67961688a37bbe.tar.gz",
)

http_archive(
    name = "io_bazel_rules_docker",
    sha256 = "31cd410896375740c1ff537dc8de409b23a28bf00c9f4b07a85d9fd14d577f88",
    strip_prefix = "rules_docker-6d9fdf2ca948ba4cfe3da778bb5afae71a3cf8dd",
    url = "https://github.com/bazelbuild/rules_docker/archive/6d9fdf2ca948ba4cfe3da778bb5afae71a3cf8dd.zip",
)

new_git_repository(
    name = "hinnant_date",
    build_file = "//:third-party/date/BUILD.bzl",
    commit = "e5c69d84ab5db3e06170b8eedec1d87841c7fb22",
    remote = "https://github.com/HowardHinnant/date.git",
)

new_git_repository(
    name = "sqlpp11",
    build_file = "//:third-party/sqlpp11/BUILD.bzl",
    commit = "ba05135d47e8674f429ba1e03a0259a48f05a4c4",
    remote = "https://github.com/rbock/sqlpp11.git",
)

new_git_repository(
    name = "sqlpp11sqlite",
    build_file = "//:third-party/sqlpp11sqlite/BUILD.bzl",
    commit = "cf37829fc9828a36afa050f960d09abcaf6aeb6a",
    remote = "https://github.com/rbock/sqlpp11-connector-sqlite3.git",
)

new_git_repository(
    name = "sqlpp11mysql",
    build_file = "//:third-party/sqlpp11mysql/BUILD.bzl",
    commit = "8a48bc2d6157fc445cda5b9beac9b7901fff625c",
    remote = "https://github.com/rbock/sqlpp11-connector-mysql.git",
)

http_archive(
    name = "org_iota_entangled",
    sha256 = "140f10b8a8b9e6f2bb1864a177cd0f30244fd565ea905e4af90fef169e3b7a00",
    strip_prefix = "entangled-1790f55de316772f55a27b8156904c1958bbe4e1",
    url = "https://github.com/iotaledger/entangled/archive/1790f55de316772f55a27b8156904c1958bbe4e1.zip",
)

new_git_repository(
    name = "iota_lib_cpp",
    build_file = "//:third-party/iota_lib_cpp/BUILD.bzl",
    commit = "9971c832e6a38972803a4d1506a78c36451c3df3",
    remote = "https://github.com/th0br0/iota.lib.cpp.git",
)

new_git_repository(
    name = "mariadb_connector",
    build_file = "//:third-party/mariadb_connector/BUILD.bzl",
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
load("@io_bazel_rules_python//python:pip.bzl", "pip_import", "pip_repositories")
load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")
load("@iota_toolchains//:toolchains.bzl", "setup_toolchains")

pip_repositories()

grpc_deps()

setup_toolchains()

iota_deps()

_cc_image_repos()

pip_import(
    name = "py_deps",
    requirements = "//:requirements.txt",
)

load("@py_deps//:requirements.bzl", "pip_install")

pip_install()
