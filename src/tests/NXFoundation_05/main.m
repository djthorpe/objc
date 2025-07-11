#include <NXFoundation/NXFoundation.h>
#include <string.h>
#include <tests/tests.h>

int main() {
  // Memory management
  NXZone *zone = [NXZone zoneWithSize:0];
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];

  // Create an object
  NXObject *obj = [[[NXObject alloc] init] autorelease];
  test_stringsequal([obj description], @"NXObject");

  // Release
  [pool release];
  [zone dealloc];

  // Return success
  return 0;
}
