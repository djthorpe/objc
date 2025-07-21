#include <NXFoundation/NXFoundation.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

int NXArchBits(void) {
#if defined(__LP64__)
  return 64;
#else
  return 32;
#endif
}

NXEndian NXArchEndian(void) {
#if __BYTE_ORDER == __BIG_ENDIAN
  return NXBigEndian;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
  return NXLittleEndian;
#else
  return NXUnknownEndian;
#endif
}
