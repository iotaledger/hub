cc_library(
    name="json",
    hdrs=["json.h"], )

cc_library(
    name="beast",
    srcs=["beast.cc"],
    hdrs=["beast.h"],
    deps=[
        ":api_json", ":json", "@boost//:beast",
        "@org_iota_entangled//common/model:transaction"
    ],
    visibility=["//visibility:public"])

cc_library(
    name="api_json",
    srcs=["api_json.cc"],
    hdrs=["api_json.h"],
    deps=[
        ":json",
        ":api",
        "@org_iota_entangled//common/helpers:digest",
        "@org_iota_entangled//common/model:transaction",
        "@com_github_google_glog//:glog",
        "@boost//:config",
        "@boost//:core",
        "@boost//:fusion",
        "@boost//:move",
        "@boost//:range"
    ], )

cc_library(
    name="api",
    hdrs=["api.h"],
    visibility=["//visibility:public"], )

cc_test(
    name="tests",
    srcs=glob(["tests/**/*.cc"]),
    deps=[
        ":beast",
        "//hub/tests:tests",
        "@com_google_googletest//:gtest",
    ])
