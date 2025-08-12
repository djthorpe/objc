#include <NXFoundation/NXFoundation.h>
#include <string.h>
#include <tests/tests.h>

int main() {
  // Create a zone, and autorelease pool
  NXZone *zone = [NXZone zoneWithSize:1024 * 64];
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];
  test_assert(pool != nil);
  test_assert([NXAutoreleasePool currentPool] == pool);

  // Add 100 objects to the pool
  for (int i = 0; i < 100; i++) {
    NXObject *obj = [[[NXObject alloc] init] autorelease];
    test_assert(obj != nil);
  }

  // Free the pool and the zone
  [pool release];
  test_assert([NXAutoreleasePool currentPool] == nil);
  [zone release];
  test_assert([NXZone defaultZone] == nil);

  // Return success
  return 0;
}
