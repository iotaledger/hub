cc_library(
    name = "auth",
    srcs = ["manager.cc"],
    hdrs = [
        "manager.h",
        "provider.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
    ],
)

cc_library(
    name = "dummy_provider",
    hdrs = ["dummy_provider.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":auth",
        "@com_github_google_glog//:glog",
    ],
)

cc_library(
    name = "hmac_provider",
    srcs = glob([
        "hmac_provider.*",
        "sign_bundle_context.*",
    ]),
    hdrs = ["hmac_provider.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":auth",
        "//common",
        "//common/crypto",
        "@argon2",
        "@com_github_google_glog//:glog",
    ],
)

cc_test(
    name = "test_hmac_provider",
    srcs = glob([
        "tests/**/*.cc",
    ]),
    deps = [
        ":hmac_provider",
        "//hub/tests",
        "@com_google_googletest//:gtest",
    ],
)
