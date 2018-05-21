cc_library(
    name = "sqlpp11mysql",
    srcs = glob([
        "src/**/*.cpp",
        "src/**/*.h",
    ]),
    hdrs = glob(["include/**"]),
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    deps = [
        "@boost//:thread",
        "@mariadb_connector//:connector",
        "@sqlpp11",
    ],
)
