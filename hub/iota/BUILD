cc_library(
    name="json",
    hdrs=["json.h"], )

cc_library(
    name="beast",
    srcs=["beast.cc"],
    hdrs=["beast.h"],
    deps=[
        ":api", ":json", "@boost//:beast",
        "@org_iota_entangled//common/model:transaction"
    ],
    visibility=["//visibility:public"])

cc_library(
    name="api",
    hdrs=["api.h"],
    visibility=["//visibility:public"], )

cc_test(
    name="tests",
    srcs=glob(["tests/**/*.cc"]),
    deps=[
        ":beast",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
    ])
