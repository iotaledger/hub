cc_library(
    name = "common",
    srcs = glob(["*.cc"]),
    hdrs = glob([
        "*.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//proto:hub_cpp",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
    ],
)
