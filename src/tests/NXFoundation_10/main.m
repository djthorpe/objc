#include <NXFoundation/NXFoundation.h>
#include <tests/tests.h>
#include <time.h>

int main() {
  printf("Running NXFoundation_10 arena allocator tests...\n");

  // Allocate a zone
  NXZone *zone = [NXZone zoneWithSize:1024 * 64];
  test_assert(zone != nil);

  // Allocate some memory in the zone
  void *ptrs[10];
  for (int i = 0; i < 10; i++) {
    ptrs[i] = [zone allocWithSize:64];
    test_assert(ptrs[i] != NULL);
    printf("Allocated %d bytes at %p\n", 64, ptrs[i]);
  }

  // Dump the zone to see allocations
  [zone dump];

  printf("Test completed successfully!\n");

  return 0;
}
