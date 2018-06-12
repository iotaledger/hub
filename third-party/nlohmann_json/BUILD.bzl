cc_library(
    name = "json",
    hdrs = glob(["include/**/*.hpp"]),
    include_prefix = "nlohmann",
    strip_include_prefix = "include/nlohmann",
    visibility = ["//visibility:public"],
)
