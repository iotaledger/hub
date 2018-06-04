#!/usr/bin/env python
import grpc

import proto.hub_pb2_grpc as hub
import proto.messages_pb2 as msg


def main():
    host = 'localhost'
    port = 50051

    with open('../ssl/client.crt', 'rb') as f:
        client_cert = f.read()
    with open('../ssl/client.key', 'rb') as f:
        client_key = f.read()
    with open('../ssl/ca.crt', 'rb') as f:
        chain = f.read()

    credentials = grpc.ssl_channel_credentials(root_certificates=chain,
                                               private_key=client_key,
                                               certificate_chain=client_cert)
    channel = grpc.secure_channel('{}:{}'.format(host, port), credentials)

    stub = hub.HubStub(channel)

    req = msg.CreateUserRequest(userId='a')
    stub.CreateUser(req)


if __name__ == '__main__':
    main()
