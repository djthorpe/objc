#include <runtime-sys/sys.h>
#include <stdint.h>
#include <tests/tests.h>

int test_sys_00(void) {
  sys_puts("Test 1: sys_malloc - basic allocation\n");
  {
    void *ptr = sys_malloc(100);
    test_assert(ptr != NULL);
    sys_free(ptr);
    sys_puts("  ✓ Basic malloc/free works\n");
  }

  sys_puts("Test 2: sys_malloc - zero size\n");
  {
    void *ptr = sys_malloc(0);
    // Zero size malloc behavior is implementation-defined, but should not crash
    if (ptr != NULL) {
      sys_free(ptr);
    }
    sys_puts("  ✓ Zero size malloc handled\n");
  }

  sys_puts("Test 3: sys_malloc - large allocation\n");
  {
#ifdef SYSTEM_NAME_PICO
    void *ptr = sys_malloc(128 * 1024); // 128KB
#else
    void *ptr = sys_malloc(1024 * 1024); // 1MB
#endif
    test_assert(ptr != NULL);
    sys_free(ptr);
    sys_puts("  ✓ Large allocation works\n");
  }

  sys_puts("Test 4: sys_malloc - multiple allocations\n");
  {
    void *ptrs[10];
    for (int i = 0; i < 10; i++) {
      ptrs[i] = sys_malloc(100 + i * 10);
      test_assert(ptrs[i] != NULL);
    }

    // Free in reverse order to test fragmentation handling
    for (int i = 9; i >= 0; i--) {
      sys_free(ptrs[i]);
    }
    sys_puts("  ✓ Multiple allocations and reverse free works\n");
  }

  sys_puts("Test 5: sys_memset - basic functionality\n");
  {
    uint8_t buffer[100];
    void *result = sys_memset(buffer, 0xAA, sizeof(buffer));
    test_assert(result == buffer);

    // Verify all bytes are set
    for (size_t i = 0; i < sizeof(buffer); i++) {
      test_assert(buffer[i] == 0xAA);
    }
    sys_puts("  ✓ Basic memset works\n");
  }

  sys_puts("Test 6: sys_memset - zero size\n");
  {
    uint8_t buffer[10] = {0x00};
    void *result = sys_memset(buffer, 0xFF, 0);
    test_assert(result == buffer);
    test_assert(buffer[0] == 0x00); // Should not be modified
    sys_puts("  ✓ Zero size memset works\n");
  }

  sys_puts("Test 7: sys_memset - different values\n");
  {
    uint8_t buffer[256];

    // Test setting to 0
    sys_memset(buffer, 0x00, sizeof(buffer));
    for (size_t i = 0; i < sizeof(buffer); i++) {
      test_assert(buffer[i] == 0x00);
    }

    // Test setting to 0xFF
    sys_memset(buffer, 0xFF, sizeof(buffer));
    for (size_t i = 0; i < sizeof(buffer); i++) {
      test_assert(buffer[i] == 0xFF);
    }

    // Test setting to 0x55
    sys_memset(buffer, 0x55, sizeof(buffer));
    for (size_t i = 0; i < sizeof(buffer); i++) {
      test_assert(buffer[i] == 0x55);
    }
    sys_puts("  ✓ Different memset values work\n");
  }

  sys_puts("Test 8: sys_memcpy - basic functionality\n");
  {
    uint8_t src[100];
    uint8_t dest[100];

    // Initialize source with pattern
    for (size_t i = 0; i < sizeof(src); i++) {
      src[i] = (uint8_t)(i & 0xFF);
    }

    void *result = sys_memcpy(dest, src, sizeof(src));
    test_assert(result == dest);

    // Verify copy
    for (size_t i = 0; i < sizeof(src); i++) {
      test_assert(dest[i] == src[i]);
    }
    sys_puts("  ✓ Basic memcpy works\n");
  }

  sys_puts("Test 9: sys_memcpy - zero size\n");
  {
    uint8_t src[10] = {0xAA};
    uint8_t dest[10] = {0x00};

    void *result = sys_memcpy(dest, src, 0);
    test_assert(result == dest);
    test_assert(dest[0] == 0x00); // Should not be modified
    sys_puts("  ✓ Zero size memcpy works\n");
  }

  sys_puts("Test 10: sys_memcpy - large data\n");
  {
    void *src = sys_malloc(1024);
    void *dest = sys_malloc(1024);
    test_assert(src != NULL && dest != NULL);

    // Initialize source with pattern
    uint8_t *src_bytes = (uint8_t *)src;
    for (size_t i = 0; i < 1024; i++) {
      src_bytes[i] = (uint8_t)(i ^ (i >> 8));
    }

    sys_memcpy(dest, src, 1024);

    // Verify copy
    uint8_t *dest_bytes = (uint8_t *)dest;
    for (size_t i = 0; i < 1024; i++) {
      test_assert(dest_bytes[i] == src_bytes[i]);
    }

    sys_free(src);
    sys_free(dest);
    sys_puts("  ✓ Large data memcpy works\n");
  }

  sys_puts("Test 11: sys_memmove - non-overlapping\n");
  {
    uint8_t src[50];
    uint8_t dest[50];

    // Initialize source
    for (size_t i = 0; i < sizeof(src); i++) {
      src[i] = (uint8_t)(i + 1);
    }

    void *result = sys_memmove(dest, src, sizeof(src));
    test_assert(result == dest);

    // Verify move
    for (size_t i = 0; i < sizeof(src); i++) {
      test_assert(dest[i] == (uint8_t)(i + 1));
    }
    sys_puts("  ✓ Non-overlapping memmove works\n");
  }

  sys_puts("Test 12: sys_memmove - overlapping forward\n");
  {
    uint8_t buffer[100];

    // Initialize buffer
    for (size_t i = 0; i < sizeof(buffer); i++) {
      buffer[i] = (uint8_t)(i & 0xFF);
    }

    // Move overlapping regions (forward)
    sys_memmove(buffer + 10, buffer, 50);

    // Verify the moved data
    for (size_t i = 0; i < 50; i++) {
      test_assert(buffer[i + 10] == (uint8_t)(i & 0xFF));
    }
    sys_puts("  ✓ Overlapping forward memmove works\n");
  }

  sys_puts("Test 13: sys_memmove - overlapping backward\n");
  {
    uint8_t buffer[100];

    // Initialize buffer
    for (size_t i = 0; i < sizeof(buffer); i++) {
      buffer[i] = (uint8_t)(i & 0xFF);
    }

    // Move overlapping regions (backward)
    sys_memmove(buffer, buffer + 10, 50);

    // Verify the moved data
    for (size_t i = 0; i < 50; i++) {
      test_assert(buffer[i] == (uint8_t)((i + 10) & 0xFF));
    }
    sys_puts("  ✓ Overlapping backward memmove works\n");
  }

  sys_puts("Test 14: sys_memcmp - equal data\n");
  {
    uint8_t data1[50];
    uint8_t data2[50];

    // Initialize both arrays with same data
    for (size_t i = 0; i < sizeof(data1); i++) {
      data1[i] = data2[i] = (uint8_t)(i * 3);
    }

    int result = sys_memcmp(data1, data2, sizeof(data1));
    test_assert(result == 0);
    sys_puts("  ✓ Equal data memcmp works\n");
  }

  sys_puts("Test 15: sys_memcmp - different data\n");
  {
    uint8_t data1[50];
    uint8_t data2[50];

    // Initialize arrays with different data
    for (size_t i = 0; i < sizeof(data1); i++) {
      data1[i] = (uint8_t)(i);
      data2[i] = (uint8_t)(i + 1);
    }

    int result = sys_memcmp(data1, data2, sizeof(data1));
    test_assert(result < 0); // data1 should be less than data2

    result = sys_memcmp(data2, data1, sizeof(data1));
    test_assert(result > 0); // data2 should be greater than data1
    sys_puts("  ✓ Different data memcmp works\n");
  }

  sys_puts("Test 16: sys_memcmp - zero size\n");
  {
    uint8_t data1[10] = {0xAA};
    uint8_t data2[10] = {0xBB};

    int result = sys_memcmp(data1, data2, 0);
    test_assert(result == 0); // Zero size should always return 0
    sys_puts("  ✓ Zero size memcmp works\n");
  }

  sys_puts("Test 17: sys_memcmp - single byte differences\n");
  {
    uint8_t data1[100];
    uint8_t data2[100];

    // Initialize with same data
    for (size_t i = 0; i < sizeof(data1); i++) {
      data1[i] = data2[i] = 0x42;
    }

    // Modify one byte in the middle
    data2[50] = 0x43;

    int result = sys_memcmp(data1, data2, sizeof(data1));
    test_assert(result < 0);

    // Test different positions
    data2[50] = 0x42; // Reset
    data2[0] = 0x41;  // Change first byte
    result = sys_memcmp(data1, data2, sizeof(data1));
    test_assert(result > 0);

    data2[0] = 0x42;  // Reset
    data2[99] = 0x43; // Change last byte
    result = sys_memcmp(data1, data2, sizeof(data1));
    test_assert(result < 0);

    sys_puts("  ✓ Single byte difference memcmp works\n");
  }

  sys_puts("Test 18: Memory allocation stress test\n");
  {
    const int num_allocs = 1000;
    void *ptrs[num_allocs];

    // Allocate many small blocks
    for (int i = 0; i < num_allocs; i++) {
      ptrs[i] = sys_malloc(16 + (i % 64));
      test_assert(ptrs[i] != NULL);
    }

    // Free every other block
    for (int i = 0; i < num_allocs; i += 2) {
      sys_free(ptrs[i]);
      ptrs[i] = NULL;
    }

    // Reallocate the freed blocks
    for (int i = 0; i < num_allocs; i += 2) {
      ptrs[i] = sys_malloc(32 + (i % 32));
      test_assert(ptrs[i] != NULL);
    }

    // Free all remaining blocks
    for (int i = 0; i < num_allocs; i++) {
      if (ptrs[i] != NULL) {
        sys_free(ptrs[i]);
      }
    }

    sys_puts("  ✓ Memory allocation stress test works\n");
  }

  sys_puts("Test 19: Memory function integration test\n");
  {
    // Allocate buffer
    void *buffer = sys_malloc(1024);
    test_assert(buffer != NULL);

    // Clear buffer
    sys_memset(buffer, 0x00, 1024);

    // Create pattern in first half
    uint8_t pattern[512];
    for (size_t i = 0; i < sizeof(pattern); i++) {
      pattern[i] = (uint8_t)(i ^ (i >> 4));
    }

    // Copy pattern to buffer
    sys_memcpy(buffer, pattern, sizeof(pattern));

    // Verify copy
    test_assert(sys_memcmp(buffer, pattern, sizeof(pattern)) == 0);

    // Move pattern to second half using memmove
    sys_memmove((uint8_t *)buffer + 512, buffer, 512);

    // Verify both halves are identical
    test_assert(sys_memcmp(buffer, (uint8_t *)buffer + 512, 512) == 0);

    sys_free(buffer);
    sys_puts("  ✓ Memory function integration works\n");
  }

  sys_puts("Test 20: Edge case - alignment testing\n");
  {
    // Test various alignments
    for (size_t offset = 0; offset < 16; offset++) {
      void *base = sys_malloc(1024 + offset);
      test_assert(base != NULL);

      uint8_t *aligned = (uint8_t *)base + offset;

      // Test memset on misaligned address
      sys_memset(aligned, 0x5A, 100);
      for (size_t i = 0; i < 100; i++) {
        test_assert(aligned[i] == 0x5A);
      }

      sys_free(base);
    }
    sys_puts("  ✓ Alignment testing works\n");
  }

  sys_puts("All memory function tests completed successfully!\n");
  return 0;
}
