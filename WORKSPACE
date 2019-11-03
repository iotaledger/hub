workspace(name = "org_iota_hub")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# EXTERNAL RULES
http_archive(
    name = "com_github_grpc_grpc",
    strip_prefix = "grpc-1.22.0",
    urls = [
        "https://github.com/grpc/grpc/archive/v1.22.0.tar.gz",
    ],
)

git_repository(
    name = "io_bazel_rules_python",
    commit = "fdbb17a4118a1728d19e638a5291b4c4266ea5b8",
    remote = "https://github.com/bazelbuild/rules_python.git",
)

git_repository(
    name = "rules_iota",
    commit = "739a65952c3cf455d00e3944cf68e42720d938ce",
    remote = "https://github.com/iotaledger/rules_iota.git",
)

git_repository(
    name = "iota_toolchains",
    commit = "4db38d66f8ce719f7b88728abd1271b8d380bf43",
    remote = "https://github.com/iotaledger/toolchains.git",
)

git_repository(
    name = "bazel_toolchains",
    commit = "b6875a7bb09b4fa1db8ea347852c0dc9ccae74ab",
    remote = "https://github.com/th0br0/bazel-toolchains.git",
)

http_archive(
    name = "io_bazel_rules_go",
    sha256 = "a82a352bffae6bee4e95f68a8d80a70e87f42c4741e6a448bec11998fcc82329",
    url =
        "https://github.com/bazelbuild/rules_go/releases/download/0.18.5/rules_go-0.18.5.tar.gz",
)

git_repository(
    name = "bazel_skylib",
    remote = "https://github.com/bazelbuild/bazel-skylib.git",
    tag = "0.8.0",
)

# DEPENDENCIES
http_archive(
    name = "com_google_protobuf",
    strip_prefix = "protobuf-b4f193788c9f0f05d7e0879ea96cd738630e5d51",
    url = "https://github.com/google/protobuf/archive/b4f193788c9f0f05d7e0879ea96cd738630e5d51.tar.gz",
)

http_archive(
    name = "io_bazel_rules_docker",
    sha256 = "aed1c249d4ec8f703edddf35cbe9dfaca0b5f5ea6e4cd9e83e99f3b0d1136c3d",
    strip_prefix = "rules_docker-0.7.0",
    urls = ["https://github.com/bazelbuild/rules_docker/archive/v0.7.0.tar.gz"],
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
    commit = "4f9f001fbd692d3b1c045280f309b61d046e7783",
    remote = "https://github.com/rbock/sqlpp11.git",
)

new_git_repository(
    name = "sqlpp11sqlite",
    build_file = "//:third-party/sqlpp11sqlite/BUILD.bzl",
    commit = "192c593f8862ee6a63a60538fbf87794bfa9464e",
    remote = "https://github.com/rbock/sqlpp11-connector-sqlite3.git",
)

new_git_repository(
    name = "sqlpp11mysql",
    build_file = "//:third-party/sqlpp11mysql/BUILD.bzl",
    commit = "5e0913c843e0cf7ede439f05373033ceb2f39d11",
    remote = "https://github.com/rbock/sqlpp11-connector-mysql.git",
)

http_archive(
    name = "org_iota_entangled",
    sha256 = "47667b88632c1f0893b268dcb1dd0591cc3443ad29a6c23d8efc0857fd1cf117",
    strip_prefix = "entangled-fd0606857514e8a2bb7da955f9f3c00dcc3b047e",
    url = "https://github.com/iotaledger/entangled/archive/fd0606857514e8a2bb7da955f9f3c00dcc3b047e.zip",
)

new_git_repository(
    name = "mariadb_connector",
    build_file = "//:third-party/mariadb_connector/BUILD.bzl",
    commit = "184a16d2f1d0bb24bea6bcf23e1604093fef8f93",
    remote = "https://github.com/MariaDB/mariadb-connector-c.git",
)

load("@rules_iota//:defs.bzl", "iota_deps")

iota_deps()

load(
    "@io_bazel_rules_docker//repositories:repositories.bzl",
    container_repositories = "repositories",
)

container_repositories()

load(
    "@io_bazel_rules_docker//container:container.bzl",
    "container_pull",
)
load(
    "@io_bazel_rules_docker//cc:image.bzl",
    _cc_image_repos = "repositories",
)
load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

load("@io_bazel_rules_python//python:pip.bzl", "pip_import", "pip_repositories")

pip_repositories()

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

load("@iota_toolchains//:toolchains.bzl", "setup_toolchains")

setup_toolchains()

_cc_image_repos()

pip_import(
    name = "py_deps",
    requirements = "//:requirements.txt",
)

load("@py_deps//:requirements.bzl", "pip_install")

pip_install()

load("@io_bazel_rules_go//go:deps.bzl", "go_register_toolchains", "go_rules_dependencies")

go_rules_dependencies()

go_register_toolchains()

container_pull(
    name = "ubuntu_base",
    registry = "index.docker.io",
    repository = "iotacafe/ubuntu",
    tag = "18.04",
)
