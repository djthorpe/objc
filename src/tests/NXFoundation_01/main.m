#include <NXFoundation/NXFoundation.h>
#include <tests/tests.h>

int main() {
  // Create a zone
  NXZone *zone = [NXZone zoneWithSize:1024];
  test_assert(zone != nil);
  test_assert([NXZone defaultZone] == zone);

  // Free the zone
  // TODO: Use an autorelease pool to ensure proper memory management
  [zone dealloc];
  test_assert([NXZone defaultZone] == nil);

  // Return success
  return 0;
}
