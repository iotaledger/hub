cc_library(
    name = "db",
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
        "//common/crypto:types",
        "//schema",
        "@boost//:uuid",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
        "@optional_lite",
        "@sqlpp11",
        "@sqlpp11mysql",
        "@sqlpp11sqlite",
    ],
)

cc_test(
    name = "test_db",
    srcs = glob([
        "tests/**/*.cc",
    ]),
    deps = [
        ":db",
        "//hub/tests",
        "@com_google_googletest//:gtest",
    ],
    timeout = "moderate"
)
