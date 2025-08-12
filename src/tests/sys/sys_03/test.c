#include <runtime-sys/sys.h>
#include <tests/tests.h>

int test_sys_03(void) {
  // Test 1: Basic functionality - should return at least 1 core
  sys_printf("\nTest 1: Testing basic core count functionality...\n");
  uint8_t cores = sys_thread_numcores();
  test_assert(cores >= 1);
  sys_printf("✓ sys_thread_numcores() returned %u cores (valid: >= 1)\n",
             cores);

  // Test 2: Consistency - multiple calls should return the same value
  sys_printf("\nTest 2: Testing consistency across multiple calls...\n");
  uint8_t cores1 = sys_thread_numcores();
  uint8_t cores2 = sys_thread_numcores();
  uint8_t cores3 = sys_thread_numcores();

  test_assert(cores1 == cores2);
  test_assert(cores2 == cores3);
  test_assert(cores1 == cores3);
  sys_printf("✓ Multiple calls consistently return %u cores\n", cores1);

  // Test 3: Reasonable bounds - should not exceed UINT8_MAX - 1
  sys_printf("\nTest 3: Testing reasonable bounds...\n");
  test_assert(cores <= UINT8_MAX - 1);
  sys_printf("✓ Core count %u is within valid uint8_t range\n", cores);

  // Test 4: Sanity check - unlikely to have more than 128 cores on typical
  // systems
  sys_printf("\nTest 4: Testing sanity bounds...\n");
  if (cores > 128) {
    sys_printf(
        "⚠️  Warning: Detected %u cores - unusually high but not invalid\n",
        cores);
  } else {
    sys_printf("✓ Core count %u is within expected range for typical systems\n",
               cores);
  }

  // Test 5: Performance test - function should be fast
  sys_printf("\nTest 5: Testing performance (1000 calls)...\n");
  for (int i = 0; i < 1000; i++) {
    uint8_t test_cores = sys_thread_numcores();
    test_assert(test_cores == cores); // Should be consistent
  }
  sys_printf("✓ 1000 calls completed successfully, all returned %u cores\n",
             cores);

  // Test 6: Information display
  sys_printf("\nTest 6: System information display...\n");
  sys_printf("Host system configuration:\n");
  sys_printf("  - CPU cores detected: %u\n", cores);
  sys_printf("  - Function return type: uint8_t (max value: %u)\n", UINT8_MAX);

  if (cores == 1) {
    sys_printf("  - Single-core system detected\n");
  } else if (cores <= 4) {
    sys_printf("  - Multi-core system (typical for consumer devices)\n");
  } else if (cores <= 16) {
    sys_printf("  - High-performance system (workstation/server class)\n");
  } else {
    sys_printf("  - Very high-performance system (server/HPC class)\n");
  }

  sys_printf("\n=== All sys_thread_numcores tests passed! ===\n");
  return 0;
}
