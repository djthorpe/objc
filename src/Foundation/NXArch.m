#include <Foundation/Foundation.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Returns the architecture bit width of the current platform.
 */
uint8_t NXArchBits(void) {
#if defined(__LP64__)
  return 64;
#else
  return 32;
#endif
}

/**
 * @brief Returns the operating system of the current platform.
 */
const char *NXArchOS(void) { return SYSTEM_NAME; }

/**
 * @brief Returns the processor/CPU type of the current platform.
 */
const char *NXArchProcessor(void) { return SYSTEM_PROCESSOR; }

/**
 * @brief Returns the byte order (endianness) of the current platform.
 */
NXEndian NXArchEndian(void) {
#if defined(__BYTE_ORDER__)
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  return NXBigEndian;
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return NXLittleEndian;
#else
  return NXUnknownEndian;
#endif
#elif defined(__BIG_ENDIAN__) || defined(__ARMEB__) || defined(__THUMBEB__) || \
    defined(__AARCH64EB__) ||                                                  \
    (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN)
  return NXBigEndian;
#elif defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) ||                      \
    defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(__i386__) ||     \
    defined(__x86_64__) ||                                                     \
    (defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN)
  return NXLittleEndian;
#else
  return NXUnknownEndian;
#endif
}

/**
 * @brief Returns the number of CPU cores available on the current platform.
 */
uint8_t NXArchNumCores(void) { return sys_thread_numcores(); }
