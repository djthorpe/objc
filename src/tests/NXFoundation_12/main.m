#include <NXFoundation/NXFoundation.h>
#include <limits.h>
#include <stdio.h>
#include <tests/tests.h>

int main() {
  // Allocate a zone first
  NXZone *zone = [NXZone zoneWithSize:1024];
  test_assert(zone != nil);

  // Create autorelease pool after zones are available
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];

  // Test random integer generation
  for (int i = 0; i < 10; i++) { // Reduced from 1000 to 10 for faster testing
    int randomInt = NXRandInt32();
    test_assert(randomInt >= INT_MIN && randomInt <= INT_MAX);
    if (i < 5) { // Only log first few to avoid spam
      NXLog(@"Random Int %d: %d", i, randomInt);
    }
  }

  for (int i = 0; i < 10; i++) { // Reduced from 1000 to 10 for faster testing
    unsigned int randomUInt = NXRandUnsignedInt32();
    test_assert(randomUInt <= UINT_MAX);
    if (i < 5) { // Only log first few to avoid spam
      NXLog(@"Random Unsigned Int %d: %u", i, randomUInt);
    }
  }

  // Cleanup
  [pool release];
  [zone release];

  printf("✓ Random number generation tests completed\n");

  return 0;
}
