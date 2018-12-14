cc_library(
    name = "commands",
    srcs = glob(
        [
            "**/*.cc",
            "*.h",
        ],
        exclude = ["tests/**/*"],
    ),
    hdrs = glob(
        ["*.h"],
        exclude = ["tests/**/*"],
    ),
    visibility = ["//visibility:public"],
    deps = [
        "//common",
        "//common/crypto:argon2_provider",
        "//proto:signing_server_grpc_cc",
    ],
)
