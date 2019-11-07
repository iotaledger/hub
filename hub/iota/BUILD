cc_library(
    name = "remote_pow",
    hdrs = ["remote_pow.h"],
    visibility = ["//visibility:public"],
    deps = [":pow"],
)

cc_library(
    name = "local_pow",
    srcs = ["local_pow.cc"],
    hdrs = ["local_pow.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":pow",
        "@org_iota_entangled//common/helpers:pow",
        "@org_iota_entangled//common/model:bundle",
        "@org_iota_entangled//common/trinary:tryte_long",
    ],
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
        ":local_pow",
        "//hub/tests",
        "@com_google_googletest//:gtest",
    ],
)
