#include "common/flags.h"

namespace common {
namespace flags {

DEFINE_string(salt, "", "Salt for local seed provider");
DEFINE_string(listenAddress, "0.0.0.0:50051", "address to listen on");
// Server side credentials
DEFINE_string(sslCert, "/dev/null", "Path to SSL certificate");
DEFINE_string(sslKey, "/dev/null", "Path to SSL certificate key");
DEFINE_string(sslCA, "/dev/null", "Path to CA root");
DEFINE_string(sslDH, "/dev/null",
              "Path to Diffie Hellman parameters (when using REST)");

DEFINE_string(authMode, "none", "credentials to use. can be {none, ssl}");
// Argon2
DEFINE_uint32(maxConcurrentArgon2Hash, 4,
              "Max number of concurrent Argon2 Hash processes");
DEFINE_uint32(argon2TCost, 4, "Time cost of Argon2");
DEFINE_uint32(argon2MCost, 1 << 17, "Memory cost of Argon2 in bytes");
DEFINE_uint32(argon2Parallelism, 1,
              "Number of threads to use in parallel for Argon2");
DEFINE_uint32(argon2Mode, 2, "Argon2 mode to use: 1=argon2i;2,else=argon2id");
DEFINE_uint32(keySecLevel, 2, "Security level for IOTA signature");

DEFINE_uint32(numThreadsHttp, 4,
              "The number of threads the http server should run");
}  // namespace flags
}  // namespace common
