#include <NXFoundation/NXFoundation.h>
#include <string.h>
#include <tests/tests.h>

int main() {
  // Memory management
  NXZone *zone = [NXZone zoneWithSize:1024];
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];

  // Create an object
  NXObject *obj = [[[NXObject alloc] init] autorelease];

  // Get the description and check it's not nil
  id description = [obj description];
  test_assert(description != nil);
  test_assert([description cStr] != NULL);
  test_assert(strcmp([description cStr], "NXObject") == 0);

  NXLog(@"DEBUG: Created object: %s", [description cStr]);

  // Release
  [pool release];
  [zone release];

  // Return success
  return 0;
}
