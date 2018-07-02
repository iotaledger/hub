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
        "//hub/crypto:argon2_provider",
        "//proto:signing_server_cpp",
    ],
)
