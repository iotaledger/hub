cc_library(
    name = "commands",
    srcs = glob(["*.cc"]),
    hdrs = glob([
        "*.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//common/crypto:types",
        "//common/stats",
        "@boost//:beast",
        "@boost//:property_tree",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
        "@com_github_grpc_grpc//:grpc++",
        "@org_iota_entangled//cppclient:beast",
    ],
)
