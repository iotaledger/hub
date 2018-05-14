cc_library(
    name = "argon2",
    srcs = [
        "src/core.c",
        "src/argon2.c",
        "src/encoding.c",
        "src/thread.c",
        "src/ref.c",
    ] + [
        "src/core.h",
        "src/encoding.h",
        "src/thread.h",
    ] + glob([
        "src/blake2/*.h",
        "src/blake2/*.c",
    ]),
    hdrs = ["include/argon2.h"],
    include_prefix = "",
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)
