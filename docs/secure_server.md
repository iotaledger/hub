# Securing Hub/Signing_Server (The example below is for hub but could easily adjusted to signing_server as well)

## Connect to Hub via SSL 
1. Create certificates, see example scripts in `docs/ssl` for further guidance. *The server CN needs to match the actual server's hostname!*
2. Start Hub in correct authMode: `--authMode ssl --sslKey docs/ssl/server.key --sslCert docs/ssl/server.crt --sslCA docs/ssl/ca.crt`
   
   **Please note that if you're running the hub via `bazel run`, these paths need to be absolute!**
3. Configure your Hub client to use SSL, see the provided Python example for further guidance. If you're using grpcc for testing, this might be helpful:
   `grpcc -a 'localhost:50051' -p proto/hub.proto --root_cert path-to/ca.crt --private_key path-to/client.key --cert_chain path-to/client.crt`
