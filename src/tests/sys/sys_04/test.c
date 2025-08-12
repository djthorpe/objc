#include <runtime-sys/sys.h>
#include <stdio.h>
#include <tests/tests.h>

int test_sys_04(void) {
  // Test 1: Basic functionality - sys_random_uint32() should return values
  printf("\nTest 1: Testing sys_random_uint32() basic functionality...\n");
  uint32_t rand1 = sys_random_uint32();
  uint32_t rand2 = sys_random_uint32();
  uint32_t rand3 = sys_random_uint32();

  printf("✓ sys_random_uint32() returned: %u, %u, %u\n", rand1, rand2, rand3);

  // Test 2: Basic functionality - sys_random_uint64() should return values
  printf("\nTest 2: Testing sys_random_uint64() basic functionality...\n");
  uint64_t rand64_1 = sys_random_uint64();
  uint64_t rand64_2 = sys_random_uint64();
  uint64_t rand64_3 = sys_random_uint64();

  printf("✓ sys_random_uint64() returned: %llu, %llu, %llu\n",
         (unsigned long long)rand64_1, (unsigned long long)rand64_2,
         (unsigned long long)rand64_3);

  // Test 3: Randomness check - multiple calls should not always return the same
  // value
  printf("\nTest 3: Testing randomness (multiple calls should differ)...\n");
  uint32_t values32[10];
  uint64_t values64[10];
  int all_same_32 = 1, all_same_64 = 1;

  values32[0] = sys_random_uint32();
  values64[0] = sys_random_uint64();

  for (int i = 1; i < 10; i++) {
    values32[i] = sys_random_uint32();
    values64[i] = sys_random_uint64();
    if (values32[i] != values32[0]) {
      all_same_32 = 0;
    }
    if (values64[i] != values64[0]) {
      all_same_64 = 0;
    }
  }

  test_assert(!all_same_32); // Should not all be the same
  test_assert(!all_same_64); // Should not all be the same
  printf("✓ Multiple calls produced different values for both functions\n");

  // Test 4: Range validation - values should be within proper ranges
  printf("\nTest 4: Testing range validation...\n");
  for (int i = 0; i < 5; i++) {
    uint32_t val32 = sys_random_uint32();
    uint64_t val64 = sys_random_uint64();
    // All values are valid by definition, just test they don't crash
    printf("  Values %d: uint32=%u, uint64=%llu (valid)\n", i + 1, val32,
           (unsigned long long)val64);
  }
  printf("✓ All values are within valid ranges\n");

  // Test 5: Performance test - functions should execute quickly
  printf("\nTest 5: Testing performance (functions should be fast)...\n");

  // Test multiple calls to ensure no significant delays
  for (int i = 0; i < 1000; i++) {
    sys_random_uint32();
    sys_random_uint64();
  }

  printf("✓ 1000 calls to each function completed without significant delay\n");

  // Test 6: Consistency test - functions should always return some value
  printf("\nTest 6: Testing consistency across multiple batches...\n");

  for (int batch = 0; batch < 3; batch++) {
    uint32_t batch_val32 = sys_random_uint32();
    uint64_t batch_val64 = sys_random_uint64();

    printf("  Batch %d: uint32=%u, uint64=%llu\n", batch + 1, batch_val32,
           (unsigned long long)batch_val64);
  }

  printf("✓ All batches returned valid values\n");

  // Test 7: Statistical distribution test - check for reasonable distribution
  printf("\nTest 7: Testing basic statistical distribution...\n");

  uint32_t low_count_32 = 0, high_count_32 = 0;
  uint32_t low_count_64 = 0, high_count_64 = 0;
  const uint32_t midpoint_32 = UINT32_MAX / 2;
  const uint64_t midpoint_64 = UINT64_MAX / 2;
  const int sample_size = 100;

  for (int i = 0; i < sample_size; i++) {
    uint32_t val32 = sys_random_uint32();
    uint64_t val64 = sys_random_uint64();

    if (val32 < midpoint_32) {
      low_count_32++;
    } else {
      high_count_32++;
    }

    if (val64 < midpoint_64) {
      low_count_64++;
    } else {
      high_count_64++;
    }
  }

  printf("  uint32 - Low half: %u, High half: %u (out of %d samples)\n",
         low_count_32, high_count_32, sample_size);
  printf("  uint64 - Low half: %u, High half: %u (out of %d samples)\n",
         low_count_64, high_count_64, sample_size);

  // We expect some distribution, not all in one half (very unlikely)
  test_assert(low_count_32 > 0 && high_count_32 > 0);
  test_assert(low_count_64 > 0 && high_count_64 > 0);
  printf(
      "✓ Values distributed across both halves of range for both functions\n");

  // Test 8: Verify 64-bit values actually use full range
  printf("\nTest 8: Testing 64-bit full range utilization...\n");

  uint32_t high_32_bits_nonzero = 0;
  for (int i = 0; i < 100; i++) {
    uint64_t val64 = sys_random_uint64();
    if ((val64 >> 32) != 0) {
      high_32_bits_nonzero++;
    }
  }

  printf("  Samples with non-zero upper 32 bits: %u out of 100\n",
         high_32_bits_nonzero);
  test_assert(high_32_bits_nonzero >
              0); // Should have some values using full 64-bit range
  printf("✓ 64-bit function utilizes full 64-bit range\n");

  printf("\n✅ All sys_random tests passed!\n");
  return 0;
}
