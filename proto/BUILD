package(default_visibility = ["//visibility:public"])

load("@org_pubref_rules_protobuf//protobuf:rules.bzl", "proto_language")
load("@org_pubref_rules_protobuf//cpp:rules.bzl", "cc_proto_library")
load("@org_pubref_rules_protobuf//go:rules.bzl", "go_proto_library")
load(
    "@org_pubref_rules_protobuf//grpc_gateway:rules.bzl",
    "GRPC_GATEWAY_DEPS",
    "grpc_gateway_proto_library",
)

filegroup(
    name = "protos",
    srcs = ["messages.proto"],
)

filegroup(
    name = "signing_server_protos",
    srcs = ["signing_server_messages.proto"],
)

cc_proto_library(
    name = "hub_cpp",
    protos = [
        "hub.proto",
        ":protos",
    ],
    verbose = 0,
    with_grpc = True,
)

cc_proto_library(
    name = "signing_server_cpp",
    protos = [
        "signing_server.proto",
        ":signing_server_protos",
    ],
    verbose = 0,
    with_grpc = True,
)

# Currently broken in rules_protobuf.
#grpc_gateway_proto_library(
#    name="gateway",
#    verbose=1,
#    protos=["gateway.proto", ":protos"],
#    visibility=["//visibility:public"], )
