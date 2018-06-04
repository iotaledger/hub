# Securing Hub

## Connect to Hub via SSL
1. Create certificates, see example scripts in `docs/ssl` for further guidance. *The server CN needs to match the actual server's hostname!*
2. Start Hub in correct authMode: `--authMode ssl --sslKey docs/ssl/server.key --sslCert docs/ssl/server.crt --sslCA docs/ssl/ca.crt`
3. Configure your Hub client to use SSL, see the provided Python example for further guidance. If you're using grpcc for testing, this might be helpful:

   `grpcc -a 'localhost:50051' -p proto/hub.proto --root_cert docs/ssl/ca.crt --private_key docs/ssl/client.key --cert_chain docs/ssl/client.crt`
