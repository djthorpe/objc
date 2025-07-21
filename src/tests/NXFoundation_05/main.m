#include <NXFoundation/NXFoundation.h>
#include <string.h>
#include <tests/tests.h>

int main() {
  // Memory management
  NXZone *zone = [NXZone zoneWithSize:1024];
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];

  // Create an object
  NXObject *obj = [[[NXObject alloc] init] autorelease];

  NXLog(@"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Created object: %s", [[obj description] cStr]);

  test_stringsequal([obj description], @"NXObject");

  // Release
  [pool release];
  [zone release];

  // Return success
  return 0;
}
