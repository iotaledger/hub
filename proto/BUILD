package(default_visibility = ["//visibility:public"])

load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

proto_library(
    name = "messages_proto",
    srcs = ["messages.proto"],
)

cc_grpc_library(
    name = "messages_proto_cc",
    srcs = ["messages.proto"],
    proto_only = True,
    well_known_protos = False,
    deps = [],
)

cc_grpc_library(
    name = "hub_grpc_cc",
    srcs = ["hub.proto"],
    generate_mocks = True,
    proto_only = False,
    well_known_protos = False,
    deps = [":messages_proto_cc"],
)

proto_library(
    name = "signing_server_messages_proto",
    srcs = ["signing_server_messages.proto"],
)

cc_grpc_library(
    name = "signing_server_messages_proto_cc",
    srcs = ["signing_server_messages.proto"],
    proto_only = True,
    well_known_protos = False,
    deps = [],
)

cc_grpc_library(
    name = "signing_server_grpc_cc",
    srcs = ["signing_server.proto"],
    proto_only = False,
    well_known_protos = False,
    deps = [":signing_server_messages_proto_cc"],
)
