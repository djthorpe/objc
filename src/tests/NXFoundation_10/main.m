#include <NXFoundation/NXFoundation.h>
#include <stdio.h>
#include <tests/tests.h>

int main() {
  NXLog(@"Starting NXFoundation arena allocator tests...\n");

  // Test 1: Basic zone creation and allocation
  NXLog(@"Test 1: Testing basic zone creation and allocation...");
  NXZone *zone = [NXZone zoneWithSize:8192];

  // Create autorelease pool after zones are available
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];
  test_assert(zone != nil);

  void *ptr1 = [zone allocWithSize:256];
  test_assert(ptr1 != NULL);

  void *ptr2 = [zone allocWithSize:512];
  test_assert(ptr2 != NULL);

  // Verify pointers are different
  test_assert(ptr1 != ptr2);

  [zone free:ptr1];
  [zone free:ptr2];
  printf("✓ Basic zone creation and allocation works\n");

  // Test 2: Arena allocator test with random sizes
  NXLog(@"Test 2: Testing arena allocator with random sizes...");

  // Allocate some memory in the zone with random sizes
  void *ptrs[100];
  int successful_allocs = 0;

  for (int i = 0; i < 100; i++) {
    size_t size = 64 + NXRandUnsignedInt32() %
                           128; // Random size between 64 and 191 bytes
    ptrs[i] = [zone allocWithSize:size];
    if (ptrs[i] != NULL) {
      successful_allocs++;
    } else {
      // Free a random existing allocation to make space
      if (successful_allocs > 0) {
        int freeIndex = (int)(NXRandUnsignedInt32() % successful_allocs);
        int actualIndex = 0;
        for (int j = 0; j < i; j++) {
          if (ptrs[j] != NULL) {
            if (actualIndex == freeIndex) {
              [zone free:ptrs[j]];
              ptrs[j] = NULL;
              successful_allocs--;
              break;
            }
            actualIndex++;
          }
        }
      }
    }
  }

  NXLog(@"Successfully allocated %d out of 100 random-sized blocks",
        successful_allocs);
  test_assert(successful_allocs > 50); // Should allocate at least half

  // Free all allocated memory
  for (int i = 0; i < 100; i++) {
    if (ptrs[i] != NULL) {
      [zone free:ptrs[i]];
      ptrs[i] = NULL;
    }
  }
  printf("✓ Arena allocator works with random sizes\n");

  // Test 3: Zone statistics and limits
  NXLog(@"Test 3: Testing zone statistics and limits...");

  size_t initial_total = [zone bytesTotal];
  size_t initial_used = [zone bytesUsed];
  size_t initial_free = [zone bytesFree];

  NXLog(@"Initial stats - Total: %zu, Used: %zu, Free: %zu", initial_total,
        initial_used, initial_free);

  // Account for reasonable internal overhead (bookkeeping, alignment, etc.)
  // The zone may have internal structures that consume some space
  size_t accounted_space = initial_used + initial_free;
  size_t overhead = initial_total - accounted_space;

  NXLog(@"Internal overhead: %zu bytes", overhead);
  test_assert(overhead < 256); // Reasonable overhead limit

  // Allocate some memory and check stats
  void *test_ptr = [zone allocWithSize:1024];
  test_assert(test_ptr != NULL);

  size_t after_alloc_used = [zone bytesUsed];
  size_t after_alloc_free = [zone bytesFree];

  test_assert(after_alloc_used > initial_used);
  test_assert(after_alloc_free < initial_free);

  [zone free:test_ptr];
  printf("✓ Zone statistics work correctly\n");

  // Test 4: Zone dump functionality
  NXLog(@"Test 4: Testing zone dump functionality...");
  void *dump_ptr = [zone allocWithSize:128];
  [zone dump]; // Should not crash
  [zone free:dump_ptr];
  printf("✓ Zone dump functionality works\n");

  // Now we can safely release the zone
  [pool release];
  [zone release];

  printf("\n=== All NXFoundation arena allocator tests passed! ===\n");

  return 0;
}
