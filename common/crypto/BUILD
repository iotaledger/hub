cc_library(
    name = "crypto",
    srcs = [
        "manager.cc",
        "provider_base.cc",
    ],
    hdrs = [
        "manager.h",
        "provider_base.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":types",
        "//proto:signing_server_grpc_cc",
        "@optional_lite",
        "@org_iota_entangled//common/helpers:checksum",
    ],
)

cc_library(
    name = "types",
    srcs = ["types.cc"],
    hdrs = [
        "types.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@boost//:random",
        "@com_github_google_glog//:glog",
    ],
)

cc_library(
    name = "argon2_provider",
    srcs = ["argon2_provider.cc"],
    hdrs = ["argon2_provider.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":crypto",
        "//common",
        "@argon2",
        "@boost//:interprocess",
        "@com_github_google_glog//:glog",
        "@iota_lib_cpp",
        "@org_iota_entangled//common/crypto/iss/v1:iss_kerl",
        "@org_iota_entangled//common/crypto/kerl:converter",
        "@org_iota_entangled//common/helpers:sign",
        "@org_iota_entangled//common/trinary:trit_tryte",
    ],
)

cc_test(
    name = "test_crypto",
    srcs = glob([
        "tests/**/*.cc",
    ]),
    deps = [
        ":argon2_provider",
        "//common/tests",
        "@boost//:interprocess",
        "@com_google_googletest//:gtest",
    ],
)
