cc_library(
    name = "bundle_utils",
    srcs = ["bundle_utils.cc"],
    hdrs = ["bundle_utils.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/crypto",
        "//hub/db",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
        "@org_iota_entangled//common/model:bundle",
        "@org_iota_entangled//common/trinary:flex_trit",
        "@org_iota_entangled//utils:bundle_miner",
    ],
)
