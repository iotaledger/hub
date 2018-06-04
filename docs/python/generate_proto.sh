#!/bin/bash

python -m grpc_tools.protoc --python_out=. --proto_path=../../ --grpc_python_out=. ../../proto/messages.proto ../../proto/hub.proto
