#include <NXFoundation/NXFoundation.h>
#include <limits.h>
#include <tests/tests.h>

int main() {
  for (int i = 0; i < 1000; i++) {
    int randomInt = NXRandInt();
    test_assert(randomInt >= INT_MIN && randomInt <= INT_MAX);
    NXLog(@"Random Int %d: %d", i, randomInt);
  }

  for (int i = 0; i < 1000; i++) {
    unsigned int randomUInt = NXRandUnsignedInt();
    test_assert(randomUInt <= UINT_MAX);
    NXLog(@"Random Unsigned Int %d: %u", i, randomUInt);
  }

  return 0;
}
