cc_library(
    name = "common",
    srcs = glob(["*.cc"]),
    hdrs = glob([
        "*.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//common/commands",
        "//common/crypto:types",
        "//common/stats",
        "//proto:hub_grpc_cc",
        "@boost//:beast",
        "@boost//:property_tree",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
        "@com_github_grpc_grpc//:grpc++",
    ],
)
