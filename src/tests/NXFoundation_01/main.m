#include <NXFoundation/NXFoundation.h>
#include <tests/tests.h>

#ifdef SYSTEM_NAME_PICO
// HACK
void *stdout = NULL;
void *stderr = NULL;
#endif

int test_NXFoundation_01(void) {
  // Create a zone
  NXZone *zone = [NXZone zoneWithSize:1024];
  test_assert(zone != nil);
  test_assert([NXZone defaultZone] == zone);

  [zone dump]; // Dump the zone for debugging

  // Free the zone
  [zone dealloc];
  test_assert([NXZone defaultZone] == nil);

  // Return success
  return 0;
}

int main(void) {
  return TestMain("test_NXFoundation_01", test_NXFoundation_01);
}
