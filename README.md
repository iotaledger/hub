# IOTA Hub
## Dependencies
- Modern GCC or Clang
- [[https://github.com/bazelbuild/bazel/releases][bazel]]
- sqlite3
- pyparsing (pip install pyparsing)
- buildifier: (`go get -u github.com/bazelbuild/buildtools/buildifier`)

## Developing RPCHub
- Be sure to run `./hooks/autohook.sh install` after checkout!

## How to run
- `bazel run -c opt //hub:hub -- <parameters>`
- `bazel build //hub:hub`
- `rm hub.db; sqlite3 hub.db < schema/schema.sql; GLOG_logtostderr=true GLOG_v=3 ./bazel-bin/hub/hub --salt abcdefghijklmnopqrstuvwxyz --apiAddress localhost:14700`

## Useful things
- Use `clang-format`
- Use `protofmt` from `go get -u -v github.com/emicklei/proto-contrib/cmd/protofmt` to format
- Use `grpcc` to test: `grpcc -i -a localhost:50051 - p proto/hub.protoc`
- Use `https://github.com/pseudomuto/protoc-gen-doc` to generate protobuf documentation
