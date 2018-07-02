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
        "//hub/commands:base_command",
        "//hub/crypto:argon2_provider",
    ],
)
