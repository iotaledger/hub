load("@io_bazel_rules_docker//cc:image.bzl", "cc_image")

cc_image(
    name = "hub",
    binary = "//hub:hub",
    repository = "iota/hub/docker",
    visibility = ["//visibility:public"],
)

cc_image(
    name = "signing_server",
    repository = "iota/hub/docker",
    binary = "//signing_server:signing_server",
    visibility = ["//visibility:public"],
)
