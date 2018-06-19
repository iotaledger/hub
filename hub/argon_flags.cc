#include "argon_flags.h"

DEFINE_uint32(maxConcurrentArgon2Hash, 4,
"Max number of concurrent Argon2 Hash processes");
DEFINE_uint32(argon2TCost, 4, "Time cost of Argon2");
DEFINE_uint32(argon2MCost, 1 << 17, "Memory cost of Argon2 in bytes");
DEFINE_uint32(argon2Parallelism, 1,
"Number of threads to use in parallel for Argon2");
DEFINE_uint32(argon2Mode, 2, "Argon2 mode to use: 1=argon2i;2,else=argon2id");

