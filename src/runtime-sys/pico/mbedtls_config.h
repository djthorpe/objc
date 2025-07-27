#pragma once

// Enable MD5 support
#define MBEDTLS_MD5_C

// Enable SHA-256 support, with hardware acceleration if available
#if LIB_PICO_SHA256
#define MBEDTLS_SHA256_ALT
#else
#define MBEDTLS_SHA256_C
#endif
