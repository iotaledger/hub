cc_library(
    name = "crypto",
    srcs = ["manager.cc"],
    hdrs = [
        "manager.h",
        "provider.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":types",
        "//hub/db",
    ],
)

cc_library(
    name = "local_provider",
    srcs = ["local_provider.cc"],
    hdrs = ["local_provider.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":crypto",
        "@argon2",
        "@com_github_google_glog//:glog",
        "@org_iota_entangled//common/helpers:sign",
        "@org_iota_entangled//common/kerl:converter",
        "@org_iota_entangled//common/trinary:trit_tryte",
    ],
)

cc_library(
    name = "types",
    srcs = ["types.cc"],
    hdrs = [
        "types.h",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "test_crypto",
    timeout = "short",
    srcs = glob([
        "tests/**/*.cc",
    ]),
    deps = [
        ":local_provider",
        "//hub/tests",
        "@com_google_googletest//:gtest",
    ],
)
