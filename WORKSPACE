workspace(name = "org_iota_hub")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# EXTERNAL RULES
http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "d6af0859d3ae4693b1955e972aa2e590d6f4d44baaa82651467c6beea453e30e",
    strip_prefix = "grpc-1.26.0-pre1",
    urls = [
        "https://github.com/grpc/grpc/archive/v1.26.0-pre1.tar.gz",
    ],
)

git_repository(
    name = "rules_python",
    commit = "38f86fb55b698c51e8510c807489c9f4e047480e",
    remote = "https://github.com/bazelbuild/rules_python.git",
)

git_repository(
    name = "rules_iota",
    commit = "e08b0038f376d6c82b80f5283bb0a86648bb58dc",
    remote = "https://github.com/iotaledger/rules_iota.git",
)

git_repository(
    name = "iota_toolchains",
    commit = "700904f445d15ef948d112bf0bccf7dd3814ae5c",
    remote = "https://github.com/iotaledger/toolchains.git",
)

git_repository(
    name = "bazel_toolchains",
    commit = "6e25ae44ec4a3b14fea9506633f3f820f3b76224",
    remote = "https://github.com/th0br0/bazel-toolchains.git",
)

http_archive(
    name = "io_bazel_rules_go",
    sha256 = "e88471aea3a3a4f19ec1310a55ba94772d087e9ce46e41ae38ecebe17935de7b",
    urls = [
        "https://storage.googleapis.com/bazel-mirror/github.com/bazelbuild/rules_go/releases/download/v0.20.3/rules_go-v0.20.3.tar.gz",
        "https://github.com/bazelbuild/rules_go/releases/download/v0.20.3/rules_go-v0.20.3.tar.gz",
    ],
)

http_archive(
    name = "com_google_protobuf",
    sha256 = "3bf61fcb22475b797aa2081512fbdcc59b786a2ca3d53b6d17d68c8e1e496bb5",
    strip_prefix = "protobuf-04a11fc91668884d1793bff2a0f72ee6ce4f5edd",
    url = "https://github.com/google/protobuf/archive/04a11fc91668884d1793bff2a0f72ee6ce4f5edd.tar.gz",
)

http_archive(
    name = "io_bazel_rules_docker",
    sha256 = "413bb1ec0895a8d3249a01edf24b82fd06af3c8633c9fb833a0cb1d4b234d46d",
    strip_prefix = "rules_docker-0.12.0",
    urls = ["https://github.com/bazelbuild/rules_docker/archive/v0.12.0.tar.gz"],
)

new_git_repository(
    name = "hinnant_date",
    build_file = "//:third-party/date/BUILD.bzl",
    commit = "48433b9892ac5741f8d4d8753beb76e49c57f7c8",
    remote = "https://github.com/HowardHinnant/date.git",
)

new_git_repository(
    name = "sqlpp11",
    build_file = "//:third-party/sqlpp11/BUILD.bzl",
    commit = "c021c49b0b1eeafc09718c7d3facb3b713eb47e4",
    remote = "https://github.com/rbock/sqlpp11.git",
)

new_git_repository(
    name = "sqlpp11sqlite",
    build_file = "//:third-party/sqlpp11sqlite/BUILD.bzl",
    commit = "dff49e0347e45a11d13b70b741ac81978d85190d",
    remote = "https://github.com/rbock/sqlpp11-connector-sqlite3.git",
)

new_git_repository(
    name = "sqlpp11mysql",
    build_file = "//:third-party/sqlpp11mysql/BUILD.bzl",
    commit = "243096a1d2619d409a4be8d869ff9d3d3f8e6ccb",
    remote = "https://github.com/rbock/sqlpp11-connector-mysql.git",
)

http_archive(
    name = "org_iota_entangled",
    sha256 = "d61c57ddd5d63e8e16d41f77bca42107081c68923f7b3d66c0e706e957152af2",
    strip_prefix = "entangled-67cd6b64e44daf383106f252e9585e161663c5d5",
    url = "https://github.com/iotaledger/entangled/archive/67cd6b64e44daf383106f252e9585e161663c5d5.zip",
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

load("@rules_python//python:pip.bzl", "pip_import", "pip_repositories")

pip_repositories()

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

grpc_extra_deps()

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
