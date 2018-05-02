cc_library(
    name = "server",
    srcs = glob(["**/*.cc"]),
    hdrs = glob(["**/*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//hub/commands",
        "//hub/db",
        "//hub/iota:beast",
        "//hub/service:attachment_service",
        "//hub/service:user_address_monitor",
        "//hub/stats",
        "//proto:cpp",
        "@boost//:uuid",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
    ],
)
