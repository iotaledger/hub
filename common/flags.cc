#include "common/flags.h"

namespace common {
namespace flags {
DEFINE_string(salt, "",
              "Salt for argon2 seed provider, should be provider here only if "
              "remoteCryptoProvider = false");

DEFINE_string(listenAddress, "0.0.0.0:50051", "address to listen on");

// Server side credentials
DEFINE_string(sslCert, "/dev/null", "Path to SSL certificate");
DEFINE_string(sslKey, "/dev/null", "Path to SSL certificate key");
DEFINE_string(sslCA, "/dev/null", "Path to CA root");
DEFINE_string(authMode, "none", "credentials to use. can be {none, ssl}");
// Argon2
DEFINE_uint32(maxConcurrentArgon2Hash, 4,
              "Max number of concurrent Argon2 Hash processes");
DEFINE_uint32(argon2TCost, 4, "Time cost of Argon2");
DEFINE_uint32(argon2MCost, 1 << 17, "Memory cost of Argon2 in bytes");
DEFINE_uint32(argon2Parallelism, 1,
              "Number of threads to use in parallel for Argon2");
DEFINE_uint32(argon2Mode, 2, "Argon2 mode to use: 1=argon2i;2,else=argon2id");
}  // namespace flags
}  // namespace common
