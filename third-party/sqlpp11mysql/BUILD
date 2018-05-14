cc_library(
    name = "sqlpp11mysql",
    srcs = glob([
        "src/**/*.cpp",
        "src/**/*.h",
    ]),
    hdrs = glob(["include/**"]),
    copts = ["-I/usr/include/mysql"],
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    deps = [
        "@mariadb_connector//:connector",
        "@sqlpp11",
    ],
)
