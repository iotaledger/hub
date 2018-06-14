cc_binary(
    name = "hub",
    srcs = ["main.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//hub/db",
        "//hub/server",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
    ],
)

cc_library(
    name = "argon_flags",
    srcs = ["argon_flags.cc"],
    hdrs = ["argon_flags.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_github_gflags_gflags//:gflags",
    ],
)
