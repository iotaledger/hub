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
        "//hub/bundle:bundle_utils",
        "//hub/db",
        "@boost//:range",
        "@com_github_google_glog//:glog",
        "@org_iota_entangled//cppclient:beast",
    ],
)

cc_test(
    name = "tests",
    timeout = "long",
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
