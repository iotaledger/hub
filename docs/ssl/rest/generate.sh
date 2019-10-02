#!/bin/bash

# in order to be able to test commands with https support using curl and a self
# signed certificate, use the `-k` flag

openssl dhparam -out dh.pem 2048
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days - nodes 365 -sub -subj "/CN=localhost"