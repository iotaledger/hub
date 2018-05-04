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
    deps = [":api"],
)

cc_library(
    name = "beast",
    srcs = ["beast.cc"],
    hdrs = ["beast.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":api_json",
        "@boost//:beast",
        "@nlohmann//:json",
        "@org_iota_entangled//common/model:transaction",
    ],
)

cc_library(
    name = "api_json",
    srcs = ["api_json.cc"],
    hdrs = ["api_json.h"],
    deps = [
        ":api",
        "@boost//:config",
        "@boost//:core",
        "@boost//:fusion",
        "@boost//:move",
        "@boost//:range",
        "@com_github_google_glog//:glog",
        "@nlohmann//:json",
        "@org_iota_entangled//common/helpers:digest",
        "@org_iota_entangled//common/model:transaction",
    ],
)

cc_library(
    name = "api",
    hdrs = ["api.h"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "tests",
    srcs = glob(["tests/**/*.cc"]),
    deps = [
        ":beast",
        "//hub/tests",
        "@com_google_googletest//:gtest",
    ],
)
