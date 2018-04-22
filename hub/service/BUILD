cc_library(
    name="service",
    srcs=["service.cc"],
    hdrs=["service.h"],
    deps=["@boost//:asio", "@boost//:bind"],
    visibility=["//visibility:public"])

cc_library(
    name="address_monitor",
    srcs=["address_monitor.cc"],
    hdrs=["address_monitor.h"],
    deps=[
        ":scheduled_service",
        "@com_github_google_glog//:glog",
    ],
    visibility=["//visibility:public"])

cc_library(
    name="user_address_monitor",
    srcs=["user_address_monitor.cc"],
    hdrs=["user_address_monitor.h"],
    deps=[":address_monitor", "//hub/db:db"],
    visibility=["//visibility:public"])

cc_library(
    name="scheduled_service",
    srcs=["scheduled_service.cc"],
    hdrs=["scheduled_service.h"],
    deps=[":service", "@boost//:asio"],
    visibility=["//visibility:public"])

cc_test(
    name="tests",
    srcs=glob(["tests/**/*.cc"]),
    deps=[
        ":scheduled_service",
        ":address_monitor",
        ":user_address_monitor",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
    ])
