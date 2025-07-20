#include <NXFoundation/NXFoundation.h>
#include <tests/tests.h>
#include <time.h>

int main() {
  printf("Running NXFoundation_10 arena allocator tests...\n");

  // Allocate a zone
  NXZone *zone = [NXZone zoneWithSize:1024];
  test_assert(zone != nil);

  // Allocate some memory in the zone
  void *ptrs[1000];
  for (int i = 0; i < 1000; i++) {
    size_t size =
        64 + NXRandUnsignedInt() % 128; // Random size between 64 and 191 bytes
    ptrs[i] = [zone allocWithSize:size];
    if (ptrs[i] == NULL) {
      // Free a random existing allocation
      size_t freeIndex = NXRandUnsignedInt() % i;
      if (ptrs[freeIndex] != NULL) {
        NXLog(@"Free @%p", ptrs[freeIndex]);
        [zone free:ptrs[freeIndex]];
        ptrs[freeIndex] = NULL; // Mark as freed
      }
    } else {
      NXLog(@"Allocated %zu bytes @%p", size, ptrs[i]);
    }
  }

  // Dump the zone to see allocations
  [zone dump];

  // Free all allocated memory
  for (int i = 0; i < 1000; i++) {
    if (ptrs[i] != NULL) {
      NXLog(@"Freeing @%p", ptrs[i]);
      [zone free:ptrs[i]];
      ptrs[i] = NULL; // Mark as freed
    }
  }

  // Free all allocated memory
  [zone release];
  test_assert([NXZone defaultZone] == nil);

  return 0;
}
