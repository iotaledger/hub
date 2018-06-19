cc_library(
    name = "tests",
    srcs = ["runner.cc"],
    hdrs = ["runner.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//hub/crypto:argon2_provider",
        "//hub/db",
        "//hub/stats",
        "@com_github_google_glog//:glog",
        "@com_google_googletest//:gtest",
    ],
)
