cc_library(
    name = "base_command",
    srcs = glob(
        ["command.h"],
        exclude = ["tests/**/*"],
    ),
    hdrs = glob(
        ["command.h"],
        exclude = ["tests/**/*"],
    ),
    visibility = ["//visibility:public"],
    deps = [
        "//common/stats",
        "//proto:hub_cpp",
    ],
)

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
        "//common",
        "//common/stats",
        "//hub/auth:hmac_provider",
        "//hub/db",
        "//proto:hub_cpp",
        "@boost//:range",
        "@com_github_google_glog//:glog",
    ],
)

cc_test(
    name = "tests",
    srcs = glob([
        "tests/**/*.cc",
        "tests/**/*.h",
    ]),
    deps = [
        ":commands",
        "//hub/tests",
        "@boost//:range",
        "@com_google_googletest//:gtest",
    ],
)
