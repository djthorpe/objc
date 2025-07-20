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
#if defined(__BIG_ENDIAN__)
  return NXBigEndian;
#elif defined(__LITTLE_ENDIAN__)
  return NXLittleEndian;
#else
  return NXUnknownEndian;
#endif
}
