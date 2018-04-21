cc_library(
    name="crypto",
    hdrs=["provider.h", "manager.h"],
    srcs=["manager.cc"],
    deps=["@boost//:uuid"],
    visibility=["//visibility:public"])

cc_library(
    name="local_provider",
    srcs=["local_provider.cc"],
    hdrs=["local_provider.h"],
    deps=[
        ":crypto", "@argon2//:argon2",
        "@org_iota_entangled//common/kerl:converter",
        "@org_iota_entangled//common/helpers:sign",
        "@org_iota_entangled//common/trinary:trit_tryte"
    ],
    visibility=["//visibility:public"])

cc_test(
    name="test_local_provider",
    deps=[
        ":local_provider", "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main"
    ],
    srcs=["tests/local_provider.cc"])
