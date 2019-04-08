load(
    "@io_bazel_rules_python//python:python.bzl",
    "py_binary",
)
load(
    "@py_deps//:requirements.bzl",
    "requirement",
)

py_binary(
    name = "ddl2cpp",
    srcs = ["ddl2cpp.py"],
    deps = [
        requirement("pyparsing"),
    ],
)

genrule(
    name = "headers",
    srcs = [
        "schema.sql",
    ],
    outs = ["schema.h"],
    cmd = """
      $(location //schema:ddl2cpp) $(location schema.sql) schema hub::db::sql
      cp schema.h $@
        """,
    tools = [":ddl2cpp"],
)

cc_library(
    name = "schema",
    hdrs = [":headers"],
    data = [
        "schema.sql",
        "triggers.mariadb.sql",
        "triggers.sqlite3.sql",
    ],
    visibility = ["//visibility:public"],
)
