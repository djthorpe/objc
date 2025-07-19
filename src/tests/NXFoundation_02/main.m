#include <NXFoundation/NXFoundation.h>
#include <tests/tests.h>

int main() {
  // Create a zone
  NXZone *zone = [NXZone zoneWithSize:1024];
  test_assert(zone != nil);
  test_assert([NXZone defaultZone] == zone);

  // Create an instance of NXObject
  NXObject *object = [[NXObject alloc] init];
  test_assert(object != nil);
  test_assert([object class] == [NXObject class]);

  // Free the object
  [object release];

  // Free the zone
  // TODO: Use an autorelease pool to ensure proper memory management
  [zone dealloc];
  test_assert([NXZone defaultZone] == nil);

  // Return success
  return 0;
}
