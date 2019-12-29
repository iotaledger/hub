#!/bin/bash

openssl genrsa -passout pass:1234 -des3 -out ca.key 4096
openssl req -new -x509 -passin pass:1234 -days 365 -key ca.key -out ca.crt -subj "/C=DE/ST=Berlin/L=Berlin/O=HUB User/OU=HUB User/CN=HUB Root CA"

