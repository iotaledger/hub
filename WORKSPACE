workspace(name="org_iota_hub")

git_repository(name="org_pubref_rules_protobuf",
               remote="https://github.com/pubref/rules_protobuf",
               tag="v0.8.2")
git_repository(name="com_github_gflags_gflags",
               commit="e292e0452fcfd5a8ae055b59052fc041cbab4abf",
               remote="https://github.com/gflags/gflags.git")
git_repository(name="com_github_google_glog",
               commit="2f493d292c92abf16ebd46cfd0cc0bf8eef5724d",
               remote="https://github.com/google/glog.git")
git_repository(name="com_google_googletest",
               commit="fdb57f85710ccb17076acb1870a881964f5e04af",
               remote="https://github.com/google/googletest.git")

new_git_repository(name="hinnant_date",
                   remote="https://github.com/HowardHinnant/date.git",
                   commit="e5c69d84ab5db3e06170b8eedec1d87841c7fb22",
                   build_file="BUILD.date")

new_git_repository(name="sqlpp11",
                   remote="https://github.com/rbock/sqlpp11.git",
                   commit="1b12fddb7a0fa23f9abd07bef86a7ad010568573",
                   build_file="BUILD.sqlpp11")

new_git_repository(
    name="sqlpp11sqlite",
    remote="https://github.com/rbock/sqlpp11-connector-sqlite3.git",
    commit="8d4ea283fa891c3f1eb7e45d012bdd4c0838e13a",
    build_file="BUILD.sqlpp11sqlite")

git_repository(name="com_github_nelhage_rules_boost",
               commit="69e96e724cbe746f63d993fa8f6bf772e1ebc5cc",
               remote="https://github.com/nelhage/rules_boost", )

new_http_archive(
    name="libsodium",
    url="https://github.com/jedisct1/libsodium/archive/1.0.16.tar.gz",
    strip_prefix="libsodium-1.0.16",
    sha256="0c14604bbeab2e82a803215d65c3b6e74bb28291aaee6236d65c699ccfe1a98c",
    build_file="BUILD.libsodium")

new_git_repository(name="azmq",
                   remote="https://github.com/zeromq/azmq.git",
                   commit="a8f54cc8c9672da7d6301fe1d2719b1436ab4816",
                   build_file="BUILD.azmq")

new_git_repository(name="libzmq",
                   remote="https://github.com/zeromq/libzmq.git",
                   tag="v4.2.5",
                   build_file="BUILD.libzmq", )

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

load("@org_pubref_rules_protobuf//cpp:rules.bzl", "cpp_proto_repositories")
cpp_proto_repositories()
