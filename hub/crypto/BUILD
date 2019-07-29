cc_library(
    name = "local_signing_provider",
    srcs = ["local_provider.cc"],
    hdrs = [
        "local_provider.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//common",
        "//common/crypto:argon2_provider",
        "//hub/db",
    ],
)

cc_library(
    name = "remote_signing_server",
    srcs = ["remote_signing_provider.cc"],
    hdrs = [
        "remote_signing_provider.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//common",
        "//common/crypto",
        "//hub/db",
        "//proto:signing_server_grpc_cc",
        "@optional_lite",
    ],
)

cc_library(
    name = "signing",
    visibility = ["//visibility:public"],
    deps = [
        ":local_signing_provider",
        ":remote_signing_server",
    ],
)

cc_test(
    name = "test_crypto",
    srcs = glob([
        "tests/**/*.cc",
    ]),
    deps = [
        ":local_signing_provider",
        "//common/tests",
        "//hub/tests",
        "@boost//:interprocess",
        "@com_google_googletest//:gtest",
    ],
)
