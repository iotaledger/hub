cc_library(
    name = "server",
    srcs = [
        "grpc.cc",
        "server.cc",
    ],
    hdrs = [
        "grpc.h",
        "server.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//common",
        "//hub/crypto:argon2_provider",
        "//signing_server/commands",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
    ],
)

cc_binary(
    name = "signing_server",
    srcs = ["main.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":server",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
    ],
)
