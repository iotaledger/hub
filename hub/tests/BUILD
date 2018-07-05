cc_library(
    name = "tests",
    srcs = ["runner.cc"],
    hdrs = ["runner.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/crypto:argon2_provider",
        "//common/stats",
        "//hub/db",
        "@com_github_google_glog//:glog",
        "@com_google_googletest//:gtest",
    ],
)
