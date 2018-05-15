cc_library(
    name = "sqlite3",
    srcs = ["sqlite3.c"],
    hdrs = [
        "sqlite3.h",
        "sqlite3ext.h",
    ],
    copts = [
        "-DSQLITE_ENABLE_STAT4",
    ],
    include_prefix = "",
    linkopts = ["-ldl"],
    visibility = ["//visibility:public"],
)
