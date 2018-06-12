cc_library(
    name = "sqlpp11sqlite",
    srcs = glob([
        "src/**/*.cpp",
        "src/**/*.h",
    ]),
    hdrs = glob(["include/**"]),
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    deps = [
        "@sqlite3",
        "@sqlpp11",
    ],
)
