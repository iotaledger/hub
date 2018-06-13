cc_library(
    name = "auth",
    srcs = ["manager.cc"],
    hdrs = [
        "manager.h",
        "provider.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
    ],
)

cc_library(
    name = "dummy_provider",
    hdrs = ["dummy_provider.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":auth",
        "@com_github_google_glog//:glog",
    ],
)
