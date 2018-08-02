load("@io_bazel_rules_docker//cc:image.bzl", "cc_image")

cc_image(
    name = "hub",
    binary = "//hub:hub",
    visibility = ["//visibility:public"],
)

cc_image(
    name = "signing_server",
    binary = "//signing_server:signing_server",
    visibility = ["//visibility:public"],
)
