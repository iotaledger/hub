cc_library(
    name = "optional_lite",
    hdrs = ["include/nonstd/optional.hpp"],
    include_prefix = "nonstd",
    strip_include_prefix = "include/nonstd",
    visibility = ["//visibility:public"],
)
