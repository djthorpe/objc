#include <NXFoundation/NXFoundation.h>
#include <sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

int NXRandInt() {
  uint32_t result = sys_random_uint32();
  if (sizeof(int) <= sizeof(uint32_t)) {
    return (int)result;
  }
  panicf("NXRandInt: int size (%zu) is larger than uint32_t size (%zu)",
         sizeof(int), sizeof(uint32_t));
}

unsigned int NXRandUnsignedInt() { return (unsigned int)NXRandInt(); }
