#include <NXFoundation/NXFoundation.h>
#include <stdio.h>
#include <tests/tests.h>

int main() {
  printf("Starting NXFoundation random number tests...\n");

  // First create a zone for allocations
  NXZone *zone = [NXZone zoneWithSize:4096];
  test_assert(zone != nil);

  // Create autorelease pool after zone is available
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];

  // Test 1: Basic random number generation
  NXLog(@"Test 1: Testing basic random number generation...");
  int32_t rand_signed = NXRandInt32();
  uint32_t rand_unsigned = NXRandUnsignedInt32();
  NXLog(@"Generated signed: %d, unsigned: %u", rand_signed, rand_unsigned);
  test_assert(rand_signed != 0 ||
              rand_unsigned != 0); // At least one should be non-zero
  printf("✓ Basic random number generation works\n");

  // Test 2: Multiple calls produce different values
  NXLog(@"Test 2: Testing that multiple calls produce different values...");
  int32_t values[100];
  int duplicates = 0;
  for (int i = 0; i < 100; i++) {
    values[i] = NXRandInt32();
  }
  // Count duplicates (should be minimal)
  for (int i = 0; i < 100; i++) {
    for (int j = i + 1; j < 100; j++) {
      if (values[i] == values[j]) {
        duplicates++;
      }
    }
  }
  NXLog(@"Found %d duplicates in 100 random numbers", duplicates);
  test_assert(duplicates < 10); // Should be very few duplicates
  printf("✓ Multiple calls produce varied values (duplicates: %d)\n",
         duplicates);

  // Test 3: Distribution test - check range coverage
  NXLog(@"Test 3: Testing distribution across ranges...");
  int positive = 0, negative = 0, zero = 0;
  for (int i = 0; i < 1000; i++) {
    int32_t val = NXRandInt32();
    if (val > 0)
      positive++;
    else if (val < 0)
      negative++;
    else
      zero++;
  }
  NXLog(@"Distribution: positive=%d, negative=%d, zero=%d", positive, negative,
        zero);
  test_assert(positive > 300 &&
              negative > 300); // Should have reasonable distribution
  printf("✓ Good distribution across positive/negative values\n");

  // Test 4: Unsigned and signed values have different ranges
  NXLog(@"Test 4: Testing signed vs unsigned value ranges...");
  int32_t signed_sum = 0;
  uint32_t unsigned_sum = 0;
  BOOL found_large_unsigned = NO;

  for (int i = 0; i < 100; i++) {
    int32_t s_val = NXRandInt32();
    uint32_t u_val = NXRandUnsignedInt32();
    signed_sum += (s_val > 0) ? 1 : 0;
    unsigned_sum += (u_val > INT32_MAX) ? 1 : 0;
    if (u_val > INT32_MAX)
      found_large_unsigned = YES;
  }

  NXLog(@"Signed positive count: %d, Large unsigned count: %u", signed_sum,
        unsigned_sum);
  test_assert(found_large_unsigned); // Should find some values > INT32_MAX
  printf("✓ Signed and unsigned ranges work correctly\n");

  // Test 5: Modulo operations work correctly for ranges
  NXLog(@"Test 5: Testing modulo operations for ranges...");
  BOOL range_test_passed = YES;
  for (int i = 0; i < 100; i++) {
    uint32_t val = NXRandUnsignedInt32() % 10; // Should be 0-9
    if (val >= 10) {
      range_test_passed = NO;
      break;
    }
  }
  test_assert(range_test_passed);
  printf("✓ Modulo operations produce correct ranges\n");

  // Test 6: Statistical uniformity test
  NXLog(@"Test 6: Testing statistical uniformity across buckets...");
  const int num_buckets = 10;
  int buckets[num_buckets];
  memset(buckets, 0, sizeof(buckets));
  const int samples = 10000;

  for (int i = 0; i < samples; i++) {
    uint32_t val = NXRandUnsignedInt32() % num_buckets;
    buckets[val]++;
  }

  // Check that each bucket got roughly equal distribution (within 20% of
  // expected)
  int expected = samples / num_buckets;
  int tolerance = expected / 5; // 20% tolerance
  BOOL uniform_distribution = YES;

  for (int i = 0; i < num_buckets; i++) {
    if (buckets[i] < (expected - tolerance) ||
        buckets[i] > (expected + tolerance)) {
      uniform_distribution = NO;
      break;
    }
  }

  NXLog(@"Expected per bucket: %d, Tolerance: ±%d", expected, tolerance);
  for (int i = 0; i < num_buckets; i++) {
    NXLog(@"Bucket %d: %d samples", i, buckets[i]);
  }

  test_assert(uniform_distribution);
  printf("✓ Statistical uniformity test passed\n");

  // Test 7: Range boundary testing
  NXLog(@"Test 7: Testing range boundaries...");
  BOOL found_max_range = NO, found_min_range = NO;

  for (int i = 0; i < 10000; i++) {
    uint32_t val = NXRandUnsignedInt32();
    if (val > 0xF0000000)
      found_max_range = YES; // Upper range
    if (val < 0x10000000)
      found_min_range = YES; // Lower range
    if (found_max_range && found_min_range)
      break;
  }

  NXLog(@"Found values in upper range: %s, lower range: %s",
        found_max_range ? "YES" : "NO", found_min_range ? "YES" : "NO");
  test_assert(found_max_range && found_min_range);
  printf("✓ Range boundary testing passed\n");

  // Test 8: Sequence independence
  NXLog(@"Test 8: Testing sequence independence...");
  int32_t prev = NXRandInt32();
  int increasing = 0, decreasing = 0;

  for (int i = 0; i < 1000; i++) {
    int32_t current = NXRandInt32();
    if (current > prev)
      increasing++;
    else if (current < prev)
      decreasing++;
    prev = current;
  }

  NXLog(@"Increasing sequences: %d, Decreasing sequences: %d", increasing,
        decreasing);
  // Both should be roughly equal (within 20% of each other)
  int diff = increasing > decreasing ? increasing - decreasing
                                     : decreasing - increasing;
  test_assert(diff < 200); // Less than 20% difference
  printf("✓ Sequence independence test passed\n");

  // Test 9: Zero occurrence frequency
  NXLog(@"Test 9: Testing zero occurrence frequency...");
  int zero_count = 0;
  int total_samples = 100000;

  for (int i = 0; i < total_samples; i++) {
    if (NXRandInt32() == 0)
      zero_count++;
  }

  double zero_frequency = (double)zero_count / total_samples;
  printf("Zero occurred %d times out of %d (expected: ~0.0000%%)\n", zero_count,
         total_samples);

  // Zero should occur very rarely (much less than 1%)
  test_assert(zero_frequency < 0.001); // Less than 0.1%
  printf("✓ Zero occurrence frequency test passed\n");

  // Test 10: Bit pattern analysis
  NXLog(@"Test 10: Testing bit pattern distribution...");
  int bit_counts[32] = {0}; // Count set bits for each position
  int sample_count = 10000;

  for (int i = 0; i < sample_count; i++) {
    uint32_t val = NXRandUnsignedInt32();
    for (int bit = 0; bit < 32; bit++) {
      if (val & (1U << bit)) {
        bit_counts[bit]++;
      }
    }
  }

  // Each bit position should be set roughly 50% of the time
  int expected_bit_count = sample_count / 2;
  int bit_tolerance = expected_bit_count / 5; // 20% tolerance
  BOOL bit_distribution_good = YES;

  for (int bit = 0; bit < 32; bit++) {
    if (bit_counts[bit] < (expected_bit_count - bit_tolerance) ||
        bit_counts[bit] > (expected_bit_count + bit_tolerance)) {
      bit_distribution_good = NO;
      break;
    }
  }

  NXLog(@"Expected bits per position: %d, Tolerance: ±%d", expected_bit_count,
        bit_tolerance);
  test_assert(bit_distribution_good);
  printf("✓ Bit pattern distribution test passed\n");

  printf("\n=== All NXFoundation random number tests passed! ===\n");

  // Clean up - release pool before releasing zone
  [pool release];
  [zone release];

  return 0;
}
