#ifndef LIBSAUROS_VERSION_
#define LIBSAUROS_VERSION_

#ifndef COMPILED_GIT_HASH
#define COMPILED_GIT_HASH "unknown"
#endif

#define LIBSAUROS_MAJOR_VERSION (0)
#define LIBSAUROS_MINOR_VERSION (0)
#define LIBSAUROS_PATCH_VERSION (0)
#define LIBSAUROS_VERSION "0.0.0"

static const char *get_build_hash() { return COMPILED_GIT_HASH; }

#endif // LIBSAUROS_VERSION_
