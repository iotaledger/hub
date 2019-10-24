load("@io_bazel_rules_docker//cc:image.bzl", "cc_image")

cc_image(
    name = "hub",
    base = "@ubuntu_base//image",
    binary = "//hub:hub",
    repository = "iota/hub/docker",
    visibility = ["//visibility:public"],
)

cc_image(
    name = "signing_server",
    base = "@ubuntu_base//image",
    binary = "//signing_server:signing_server",
    repository = "iota/hub/docker",
    visibility = ["//visibility:public"],
)
