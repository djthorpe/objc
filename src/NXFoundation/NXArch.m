#include <NXFoundation/NXFoundation.h>
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
 * @brief Returns the byte order (endianness) of the current platform.
 */
NXEndian NXArchEndian(void) {
#if __BYTE_ORDER == __BIG_ENDIAN
  return NXBigEndian;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
  return NXLittleEndian;
#else
  return NXUnknownEndian;
#endif
}

/**
 * @brief Returns the number of CPU cores available on the current platform.
 */
uint8_t NXArchNumCores(void) { return sys_thread_numcores(); }
