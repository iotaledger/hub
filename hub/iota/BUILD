cc_library(
    name = "remote_pow",
    hdrs = ["remote_pow.h"],
    visibility = ["//visibility:public"],
    deps = [":pow"],
)

cc_library(
    name = "pow",
    srcs = ["pow.cc"],
    hdrs = ["pow.h"],
    visibility = ["//visibility:public"],
    deps = ["@org_iota_entangled//cppclient:beast"],
)

cc_test(
    name = "tests",
    srcs = glob(["tests/**/*.cc"]),
    deps = [
        "//hub/tests",
        "@com_google_googletest//:gtest",
    ],
)
