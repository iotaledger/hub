cc_library(
    name = "commands",
    srcs = glob(
        ["**/*.cc"],
        exclude = ["tests/**/*"],
    ),
    hdrs = glob(
        ["**/*.h"],
        exclude = ["tests/**/*"],
    ),
    visibility = ["//visibility:public"],
    deps = [
        "//hub/crypto",
        "//hub/crypto:local_provider",
        "//hub/db",
        "//hub/stats",
        "//proto:cpp",
    ],
)

cc_test(
    name = "tests",
    timeout = "short",
    srcs = glob([
        "tests/**/*.cc",
        "tests/**/*.h",
    ]),
    deps = [
        ":commands",
        "//hub/tests",
        "@com_google_googletest//:gtest",
    ],
)
