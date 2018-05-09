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
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
        "@sqlpp11",
        "@sqlpp11sqlite",
    ],
)
