#include <runtime-sys/sys.h>
#include <string.h>
#include <tests/tests.h>

// Forward declaration
int test_sys_05(void);

int main(void) {
  // Run hash function tests
  return TestMain("test_sys_05", test_sys_05);
}

int test_sys_05(void) {
  sys_printf("Test 1: Hash initialization - MD5\n");
  {
    sys_hash_t hash = sys_hash_init(sys_hash_md5);
    test_assert(sys_hash_size(&hash) == 16); // MD5 is 128 bits = 16 bytes
    // Cleanup is handled by sys_hash_finalize, but we need to call it to clean
    // up
    sys_hash_finalize(&hash);
    sys_printf("  ✓ MD5 initialization works\n");
  }

  sys_printf("Test 2: Hash initialization - SHA256\n");
  {
    sys_hash_t hash = sys_hash_init(sys_hash_sha256);
    test_assert(sys_hash_size(&hash) == 32); // SHA-256 is 256 bits = 32 bytes
    // Cleanup is handled by sys_hash_finalize, but we need to call it to clean
    // up
    sys_hash_finalize(&hash);
    sys_printf("  ✓ SHA-256 initialization works\n");
  }

  sys_printf("Test 3: MD5 hash of empty string\n");
  {
    sys_hash_t hash = sys_hash_init(sys_hash_md5);
    const uint8_t *result = sys_hash_finalize(&hash);
    test_assert(result != NULL);

    // MD5 of empty string: d41d8cd98f00b204e9800998ecf8427e
    uint8_t expected[] = {0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
                          0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e};
    test_assert(memcmp(result, expected, 16) == 0);
    sys_printf("  ✓ MD5 empty string hash correct\n");
  }

  sys_printf("Test 4: SHA-256 hash of empty string\n");
  {
    sys_hash_t hash = sys_hash_init(sys_hash_sha256);
    const uint8_t *result = sys_hash_finalize(&hash);
    test_assert(result != NULL);

    // SHA-256 of empty string:
    // e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
    uint8_t expected[] = {0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,
                          0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
                          0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
                          0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55};
    test_assert(memcmp(result, expected, 32) == 0);
    sys_printf("  ✓ SHA-256 empty string hash correct\n");
  }

  sys_printf("Test 5: MD5 hash of 'abc'\n");
  {
    sys_hash_t hash = sys_hash_init(sys_hash_md5);
    const char *input = "abc";
    test_assert(sys_hash_update(&hash, input, strlen(input)) == true);
    const uint8_t *result = sys_hash_finalize(&hash);
    test_assert(result != NULL);

    // MD5 of "abc": 900150983cd24fb0d6963f7d28e17f72
    uint8_t expected[] = {0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0,
                          0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72};
    test_assert(memcmp(result, expected, 16) == 0);
    sys_printf("  ✓ MD5 'abc' hash correct\n");
  }

  sys_printf("Test 6: SHA-256 hash of 'abc'\n");
  {
    sys_hash_t hash = sys_hash_init(sys_hash_sha256);
    const char *input = "abc";
    test_assert(sys_hash_update(&hash, input, strlen(input)) == true);
    const uint8_t *result = sys_hash_finalize(&hash);
    test_assert(result != NULL);

    // SHA-256 of "abc":
    // ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
    uint8_t expected[] = {0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
                          0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
                          0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
                          0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad};
    test_assert(memcmp(result, expected, 32) == 0);
    sys_printf("  ✓ SHA-256 'abc' hash correct\n");
  }

  sys_printf("Test 7: Multiple updates - MD5\n");
  {
    sys_hash_t hash = sys_hash_init(sys_hash_md5);

    // Hash "abc" in multiple parts: "a" + "b" + "c"
    test_assert(sys_hash_update(&hash, "a", 1) == true);
    test_assert(sys_hash_update(&hash, "b", 1) == true);
    test_assert(sys_hash_update(&hash, "c", 1) == true);

    const uint8_t *result = sys_hash_finalize(&hash);
    test_assert(result != NULL);

    // Should be same as MD5 of "abc"
    uint8_t expected[] = {0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0,
                          0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72};
    test_assert(memcmp(result, expected, 16) == 0);
    sys_printf("  ✓ Multiple updates work correctly\n");
  }

  sys_printf("Test 8: Large data hashing - SHA-256\n");
  {
    sys_hash_t hash = sys_hash_init(sys_hash_sha256);

    // Create 1000 'A' characters
    char large_data[1001];
    sys_memset(large_data, 'A', 1000);
    large_data[1000] = '\0';

    test_assert(sys_hash_update(&hash, large_data, 1000) == true);
    const uint8_t *result = sys_hash_finalize(&hash);
    test_assert(result != NULL);
    test_assert(sys_hash_size(&hash) == 32);
    sys_printf("  ✓ Large data hashing works\n");
  }

  sys_printf("Test 9: Binary data hashing\n");
  {
    sys_hash_t hash = sys_hash_init(sys_hash_sha256);

    // Create binary data with all byte values
    uint8_t binary_data[256];
    for (int i = 0; i < 256; i++) {
      binary_data[i] = (uint8_t)i;
    }

    test_assert(sys_hash_update(&hash, binary_data, 256) == true);
    const uint8_t *result = sys_hash_finalize(&hash);
    test_assert(result != NULL);
    sys_printf("  ✓ Binary data hashing works\n");
  }

  sys_printf("Test 10: Error handling - NULL data\n");
  {
    sys_hash_t hash = sys_hash_init(sys_hash_md5);

    // Test with NULL data
    test_assert(sys_hash_update(&hash, NULL, 10) == false);

    // Test with zero size (should succeed)
    test_assert(sys_hash_update(&hash, "test", 0) == true);

    // Cleanup context
    sys_hash_finalize(&hash);
    sys_printf("  ✓ NULL data handling works\n");
  }

  sys_printf("Test 11: Error handling - invalid context\n");
  {
    sys_hash_t hash = {0}; // Uninitialized hash

    // Should fail with uninitialized context
    test_assert(sys_hash_update(&hash, "test", 4) == false);
    test_assert(sys_hash_finalize(&hash) == NULL);
    test_assert(sys_hash_size(&hash) == 0);
    sys_printf("  ✓ Invalid context handling works\n");
  }

  sys_printf("Test 12: Hash size after finalization\n");
  {
    sys_hash_t hash = sys_hash_init(sys_hash_sha256);
    test_assert(sys_hash_update(&hash, "test", 4) == true);

    // Size should be available before finalization
    test_assert(sys_hash_size(&hash) == 32);

    const uint8_t *result = sys_hash_finalize(&hash);
    test_assert(result != NULL);

    // Size should still be available after finalization
    test_assert(sys_hash_size(&hash) == 32);
    sys_printf("  ✓ Hash size persistence works\n");
  }

  sys_printf("Test 13: Finalization behavior\n");
  {
    sys_hash_t hash = sys_hash_init(sys_hash_md5);
    test_assert(sys_hash_update(&hash, "partial", 7) == true);

    // Finalize the hash
    const uint8_t *result = sys_hash_finalize(&hash);
    test_assert(result != NULL);

    // Context should be unusable after finalization
    test_assert(sys_hash_update(&hash, "more", 4) == false);
    test_assert(sys_hash_finalize(&hash) == NULL);
    sys_printf("  ✓ Cleanup without finalization works\n");
  }

  sys_printf("Test 14: Known test vectors - SHA-256\n");
  {
    // Test with NIST test vector:
    // "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
    sys_hash_t hash = sys_hash_init(sys_hash_sha256);
    const char *input =
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    test_assert(sys_hash_update(&hash, input, strlen(input)) == true);
    const uint8_t *result = sys_hash_finalize(&hash);
    test_assert(result != NULL);

    // Expected SHA-256:
    // 248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1
    uint8_t expected[] = {0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
                          0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
                          0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
                          0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1};
    test_assert(memcmp(result, expected, 32) == 0);
    sys_printf("  ✓ NIST test vector correct\n");
  }

  sys_printf("Test 15: Context reuse after finalization\n");
  {
    sys_hash_t hash = sys_hash_init(sys_hash_md5);
    test_assert(sys_hash_update(&hash, "first", 5) == true);
    const uint8_t *result1 = sys_hash_finalize(&hash);
    test_assert(result1 != NULL);

    // Context should be unusable after finalization
    test_assert(sys_hash_update(&hash, "second", 6) == false);
    const uint8_t *result2 = sys_hash_finalize(&hash);
    test_assert(result2 == NULL);
    sys_printf("  ✓ Context reuse prevention works\n");
  }

  sys_printf("All hash function tests completed successfully!\n");
  return 0;
}
