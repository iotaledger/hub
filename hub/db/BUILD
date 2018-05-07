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
        "//schema",
        "@boost//:uuid",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
        "@sqlpp11",
        "@sqlpp11sqlite",
    ],
)

cc_test(
    name = "test_helper",
    timeout = "short",
    srcs = ["tests/uuid.cc"],
    deps = [
        ":db",
        "//hub/tests",
        "@com_google_googletest//:gtest",
    ],
)
