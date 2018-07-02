#include "common/flags.h"

namespace common{
namespace flags{
    DEFINE_string(salt, "",
    "Salt for argon2 seed provider, should be provider here only if "
    "remoteCryptoProvider = false");

    DEFINE_string(listenAddress, "0.0.0.0:50051", "address to listen on");

    DEFINE_string(sslCert, "/dev/null", "Path to SSL certificate");
    DEFINE_string(sslKey, "/dev/null", "Path to SSL certificate key");
    DEFINE_string(sslCA, "/dev/null", "Path to CA root");
    DEFINE_string(authMode, "none", "credentials to use. can be {none, ssl}");
}
}

