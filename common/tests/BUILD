cc_library(
    name = "tests",
    srcs = ["runner.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/crypto:argon2_provider",
        "//common/stats",
        "@com_github_google_glog//:glog",
        "@com_google_googletest//:gtest",
    ],
)
