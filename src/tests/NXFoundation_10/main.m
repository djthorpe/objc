#include <NXFoundation/NXFoundation.h>
#include <tests/tests.h>
#include <time.h>

int main() {
  printf("Running NXFoundation_10 arena allocator tests...\n");

  // Allocate a zone
  NXZone *zone = [NXZone zoneWithSize:1024];
  test_assert(zone != nil);

  printf("Test completed successfully!\n");

  return 0;
}
