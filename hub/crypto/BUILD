cc_library(
    name = "crypto",
    srcs = ["manager.cc"],
    hdrs = [
        "manager.h",
        "provider.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//hub/db",
        "@boost//:uuid",
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

cc_test(
    name = "test_local_provider",
    timeout = "short",
    srcs = ["tests/local_provider.cc"],
    deps = [
        ":local_provider",
        "//hub/tests",
        "@com_google_googletest//:gtest",
    ],
)
