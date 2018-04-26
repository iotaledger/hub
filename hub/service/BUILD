cc_library(
    name = "service",
    srcs = ["service.cc"],
    hdrs = ["service.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@boost//:asio",
        "@boost//:bind",
    ],
)

cc_library(
    name = "address_monitor",
    srcs = ["address_monitor.cc"],
    hdrs = ["address_monitor.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":scheduled_service",
        "//hub/iota:api",
        "@com_github_google_glog//:glog",
    ],
)

cc_library(
    name = "user_address_monitor",
    srcs = ["user_address_monitor.cc"],
    hdrs = ["user_address_monitor.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":address_monitor",
        "//hub/db",
        "@boost//:move",
        "@boost//:multi_index",
        "@boost//:range",
        "@boost//:move",
        "@boost//:uuid"
    ],
)

cc_library(
    name = "scheduled_service",
    srcs = ["scheduled_service.cc"],
    hdrs = ["scheduled_service.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":service",
        "@boost//:asio",
        "@com_github_google_glog//:glog",
    ],
)

cc_test(
    name = "tests",
    srcs = glob(["tests/**/*.cc"]),
    deps = [
        ":address_monitor",
        ":scheduled_service",
        ":user_address_monitor",
        "//hub/tests",
        "@com_google_googletest//:gtest",
    ],
)
