#include <NXFoundation/NXFoundation.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

int test_data_methods(void);

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) {
  NXZone *zone = [NXZone zoneWithSize:2048];
  test_assert(zone != nil);
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];
  test_assert(pool != nil);

  // Run the test for data methods
  int returnValue = TestMain("NXFoundation_22", test_data_methods);

  // Clean up
  [pool release];
  [zone release];

  // Return the result of the test
  return returnValue;
}

///////////////////////////////////////////////////////////////////////////////
// TESTS

int test_data_methods(void) {
  printf("Test 1: Basic initialization\n");
  {
    NXData *data = [[NXData alloc] init];
    test_assert(data != nil);
    test_assert([data size] == 0);
    test_assert([data capacity] == 0);
    test_assert([data bytes] == NULL);
    [data release];
    printf("  ✓ Empty initialization works\n");
  }

  printf("Test 2: Factory method +new\n");
  {
    NXData *data = [NXData new];
    test_assert(data != nil);
    test_assert([data size] == 0);
    test_assert([data capacity] == 0);
    test_assert([data bytes] == NULL);
    printf("  ✓ Factory +new works\n");
  }

  printf("Test 3: initWithCapacity - normal case\n");
  {
    NXData *data = [[NXData alloc] initWithCapacity:100];
    test_assert(data != nil);
    test_assert([data size] == 0);
    test_assert([data capacity] == 100);
    test_assert([data bytes] != NULL);
    [data release];
    printf("  ✓ Normal capacity initialization works\n");
  }

  printf("Test 4: initWithCapacity - zero capacity\n");
  {
    NXData *data = [[NXData alloc] initWithCapacity:0];
    test_assert(data != nil);
    test_assert([data size] == 0);
    test_assert([data capacity] == 0);
    test_assert([data bytes] == NULL);
    [data release];
    printf("  ✓ Zero capacity initialization works\n");
  }

  printf("Test 5: Factory method +dataWithCapacity\n");
  {
    NXData *data = [NXData dataWithCapacity:50];
    test_assert(data != nil);
    test_assert([data size] == 0);
    test_assert([data capacity] == 50);
    test_assert([data bytes] != NULL);
    printf("  ✓ Factory +dataWithCapacity works\n");
  }

  printf("Test 6: initWithString - normal string\n");
  {
    NXData *data = [[NXData alloc] initWithString:@"Hello"];
    test_assert(data != nil);
    test_assert([data size] == 6); // 5 chars + null terminator
    test_assert([data capacity] == 6);
    test_assert([data bytes] != NULL);

    const char *bytes = (const char *)[data bytes];
    test_cstrings_equal(bytes, "Hello");
    [data release];
    printf("  ✓ String initialization works\n");
  }

  printf("Test 7: initWithString - empty string\n");
  {
    NXData *data = [[NXData alloc] initWithString:@""];
    test_assert(data != nil);
    test_assert([data size] == 0);
    test_assert([data capacity] == 0);
    test_assert([data bytes] == NULL);
    [data release];
    printf("  ✓ Empty string initialization works\n");
  }

  printf("Test 8: Factory method +dataWithString\n");
  {
    NXData *data = [NXData dataWithString:@"Test"];
    test_assert(data != nil);
    test_assert([data size] == 5); // 4 chars + null terminator
    test_assert([data capacity] == 5);

    const char *bytes = (const char *)[data bytes];
    test_cstrings_equal(bytes, "Test");
    printf("  ✓ Factory +dataWithString works\n");
  }

  printf("Test 9: initWithBytes - normal case\n");
  {
    char testBytes[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:5];
    test_assert(data != nil);
    test_assert([data size] == 5);
    test_assert([data capacity] == 5);
    test_assert([data bytes] != NULL);

    const char *bytes = (const char *)[data bytes];
    test_assert(bytes[0] == 0x01);
    test_assert(bytes[1] == 0x02);
    test_assert(bytes[2] == 0x03);
    test_assert(bytes[3] == 0x04);
    test_assert(bytes[4] == 0x05);
    [data release];
    printf("  ✓ Bytes initialization works\n");
  }

  printf("Test 10: initWithBytes - zero size\n");
  {
    char testBytes[] = {0x01, 0x02};
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:0];
    test_assert(data != nil);
    test_assert([data size] == 0);
    test_assert([data capacity] == 0);
    test_assert([data bytes] == NULL);
    [data release];
    printf("  ✓ Zero size bytes initialization works\n");
  }

  printf("Test 11: Factory method +dataWithBytes\n");
  {
    char testBytes[] = {0xAA, 0xBB, 0xCC};
    NXData *data = [NXData dataWithBytes:testBytes size:3];
    test_assert(data != nil);
    test_assert([data size] == 3);
    test_assert([data capacity] == 3);

    const char *bytes = (const char *)[data bytes];
    test_assert(bytes[0] == (char)0xAA);
    test_assert(bytes[1] == (char)0xBB);
    test_assert(bytes[2] == (char)0xCC);
    printf("  ✓ Factory +dataWithBytes works\n");
  }

  printf("Test 12: Memory management - multiple allocations\n");
  {
    // Test that we can create and destroy multiple instances
    for (int i = 0; i < 10; i++) {
      NXData *data = [[NXData alloc] initWithCapacity:100 + i];
      test_assert(data != nil);
      test_assert([data capacity] == (size_t)(100 + i));
      [data release];
    }
    printf("  ✓ Multiple allocations work\n");
  }

  printf("Test 13: String with special characters\n");
  {
    NXData *data = [[NXData alloc] initWithString:@"Test\n\t\\"];
    test_assert(data != nil);
    test_assert([data size] ==
                8); // Test\n\t\\ is 7 characters + null terminator = 8

    const char *bytes = (const char *)[data bytes];
    test_cstrings_equal(bytes, "Test\n\t\\");
    [data release];
    printf("  ✓ Special characters in string work\n");
  }

  printf("Test 14: Large capacity allocation\n");
  {
    NXData *data = [[NXData alloc] initWithCapacity:4096];
    test_assert(data != nil);
    test_assert([data size] == 0);
    test_assert([data capacity] == 4096);
    test_assert([data bytes] != NULL);
    [data release];
    printf("  ✓ Large capacity allocation works\n");
  }

  printf("Test 15: Binary data with null bytes\n");
  {
    char testBytes[] = {0x00, 0x01, 0x00, 0x02, 0x00};
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:5];
    test_assert(data != nil);
    test_assert([data size] == 5);

    const char *bytes = (const char *)[data bytes];
    test_assert(bytes[0] == 0x00);
    test_assert(bytes[1] == 0x01);
    test_assert(bytes[2] == 0x00);
    test_assert(bytes[3] == 0x02);
    test_assert(bytes[4] == 0x00);
    [data release];
    printf("  ✓ Binary data with null bytes works\n");
  }

  printf("Test 16: Accessor methods consistency\n");
  {
    char testBytes[] = {0x10, 0x20, 0x30};
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:3];

    // Test that accessors return consistent values
    test_assert([data size] == 3);
    test_assert([data capacity] == 3);
    test_assert([data bytes] != NULL);

    // Test that values don't change on multiple calls
    test_assert([data size] == [data size]);
    test_assert([data capacity] == [data capacity]);
    test_assert([data bytes] == [data bytes]);

    [data release];
    printf("  ✓ Accessor methods are consistent\n");
  }

  printf("Test 17: String with Unicode-like content\n");
  {
    NXData *data = [[NXData alloc] initWithString:@"café"];
    test_assert(data != nil);
    test_assert([data size] > 0);
    test_assert([data bytes] != NULL);
    [data release];
    printf("  ✓ Unicode-like string content works\n");
  }

  printf("Test 18: Maximum reasonable capacity\n");
  {
    // Test a reasonably large but not excessive capacity
    NXData *data = [[NXData alloc] initWithCapacity:1024 * 1024]; // 1MB
    test_assert(data != nil);
    test_assert([data capacity] == 1024 * 1024);
    test_assert([data size] == 0);
    [data release];
    printf("  ✓ Large capacity (1MB) allocation works\n");
  }

  printf("Test 19: Edge case - single byte\n");
  {
    char singleByte = 0xFF;
    NXData *data = [[NXData alloc] initWithBytes:&singleByte size:1];
    test_assert(data != nil);
    test_assert([data size] == 1);
    test_assert([data capacity] == 1);

    const char *bytes = (const char *)[data bytes];
    test_assert(bytes[0] == (char)0xFF);
    [data release];
    printf("  ✓ Single byte data works\n");
  }

  printf("Test 20: hexString - empty data\n");
  {
    NXData *data = [[NXData alloc] init];
    NXString *hexStr = [data hexString];
    test_assert(hexStr != nil);
    test_cstrings_equal([hexStr cStr], "");
    [data release];
    printf("  ✓ hexString for empty data returns empty string\n");
  }

  printf("Test 21: hexString - single byte\n");
  {
    char singleByte = 0xAB;
    NXData *data = [[NXData alloc] initWithBytes:&singleByte size:1];
    NXString *hexStr = [data hexString];
    test_assert(hexStr != nil);
    test_cstrings_equal([hexStr cStr], "AB");
    [data release];
    printf("  ✓ hexString for single byte works\n");
  }

  printf("Test 22: hexString - multiple bytes\n");
  {
    char testBytes[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:8];
    NXString *hexStr = [data hexString];
    test_assert(hexStr != nil);
    test_cstrings_equal([hexStr cStr], "0123456789ABCDEF");
    [data release];
    printf("  ✓ hexString for multiple bytes works\n");
  }

  printf("Test 23: hexString - zero bytes\n");
  {
    char testBytes[] = {0x00, 0x00, 0x00};
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:3];
    NXString *hexStr = [data hexString];
    test_assert(hexStr != nil);
    test_cstrings_equal([hexStr cStr], "000000");
    [data release];
    printf("  ✓ hexString for zero bytes works\n");
  }

  printf("Test 24: hexString - all possible byte values\n");
  {
    char testBytes[] = {0x00, 0x0F, 0xF0, 0xFF};
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:4];
    NXString *hexStr = [data hexString];
    test_assert(hexStr != nil);
    test_cstrings_equal([hexStr cStr], "000FF0FF");
    [data release];
    printf("  ✓ hexString for edge byte values works\n");
  }

  printf("Test 25: hexString - string data\n");
  {
    NXData *data = [[NXData alloc] initWithString:@"Hi"];
    NXString *hexStr = [data hexString];
    test_assert(hexStr != nil);
    // "Hi" + null terminator = 0x48, 0x69, 0x00
    test_cstrings_equal([hexStr cStr], "486900");
    [data release];
    printf("  ✓ hexString for string data works\n");
  }

  printf("Test 26: hexString - consistency check\n");
  {
    char testBytes[] = {0xDE, 0xAD, 0xBE, 0xEF};
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:4];

    // Call hexString multiple times to ensure consistency
    NXString *hexStr1 = [data hexString];
    NXString *hexStr2 = [data hexString];
    test_assert(hexStr1 != nil);
    test_assert(hexStr2 != nil);
    test_cstrings_equal([hexStr1 cStr], "DEADBEEF");
    test_cstrings_equal([hexStr2 cStr], "DEADBEEF");
    test_cstrings_equal([hexStr1 cStr], [hexStr2 cStr]);

    [data release];
    printf("  ✓ hexString returns consistent results\n");
  }

  printf("Test 27: base64String - empty data\n");
  {
    NXData *data = [[NXData alloc] init];
    NXString *b64Str = [data base64String];
    test_assert(b64Str != nil);
    test_cstrings_equal([b64Str cStr], "");
    [data release];
    printf("  ✓ base64String for empty data returns empty string\n");
  }

  printf("Test 28: base64String - single byte\n");
  {
    char singleByte = 'A'; // 0x41
    NXData *data = [[NXData alloc] initWithBytes:&singleByte size:1];
    NXString *b64Str = [data base64String];
    test_assert(b64Str != nil);
    test_cstrings_equal([b64Str cStr], "QQ==");
    [data release];
    printf("  ✓ base64String for single byte works\n");
  }

  printf("Test 29: base64String - two bytes\n");
  {
    char testBytes[] = {'A', 'B'}; // 0x41, 0x42
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:2];
    NXString *b64Str = [data base64String];
    test_assert(b64Str != nil);
    test_cstrings_equal([b64Str cStr], "QUI=");
    [data release];
    printf("  ✓ base64String for two bytes works\n");
  }

  printf("Test 30: base64String - three bytes (no padding)\n");
  {
    char testBytes[] = {'A', 'B', 'C'}; // 0x41, 0x42, 0x43
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:3];
    NXString *b64Str = [data base64String];
    test_assert(b64Str != nil);
    test_cstrings_equal([b64Str cStr], "QUJD");
    [data release];
    printf("  ✓ base64String for three bytes (no padding) works\n");
  }

  printf("Test 31: base64String - four bytes\n");
  {
    char testBytes[] = {'A', 'B', 'C', 'D'}; // 0x41, 0x42, 0x43, 0x44
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:4];
    NXString *b64Str = [data base64String];
    test_assert(b64Str != nil);
    test_cstrings_equal([b64Str cStr], "QUJDRA==");
    [data release];
    printf("  ✓ base64String for four bytes works\n");
  }

  printf("Test 32: base64String - RFC 4648 test vectors\n");
  {
    // Test vector 1: "f" -> "Zg=="
    NXData *data1 = [[NXData alloc] initWithBytes:"f" size:1];
    NXString *b64Str1 = [data1 base64String];
    test_assert(b64Str1 != nil);
    test_cstrings_equal([b64Str1 cStr], "Zg==");
    [data1 release];

    // Test vector 2: "fo" -> "Zm8="
    NXData *data2 = [[NXData alloc] initWithBytes:"fo" size:2];
    NXString *b64Str2 = [data2 base64String];
    test_assert(b64Str2 != nil);
    test_cstrings_equal([b64Str2 cStr], "Zm8=");
    [data2 release];

    // Test vector 3: "foo" -> "Zm9v"
    NXData *data3 = [[NXData alloc] initWithBytes:"foo" size:3];
    NXString *b64Str3 = [data3 base64String];
    test_assert(b64Str3 != nil);
    test_cstrings_equal([b64Str3 cStr], "Zm9v");
    [data3 release];

    // Test vector 4: "foob" -> "Zm9vYg=="
    NXData *data4 = [[NXData alloc] initWithBytes:"foob" size:4];
    NXString *b64Str4 = [data4 base64String];
    test_assert(b64Str4 != nil);
    test_cstrings_equal([b64Str4 cStr], "Zm9vYg==");
    [data4 release];

    // Test vector 5: "fooba" -> "Zm9vYmE="
    NXData *data5 = [[NXData alloc] initWithBytes:"fooba" size:5];
    NXString *b64Str5 = [data5 base64String];
    test_assert(b64Str5 != nil);
    test_cstrings_equal([b64Str5 cStr], "Zm9vYmE=");
    [data5 release];

    // Test vector 6: "foobar" -> "Zm9vYmFy"
    NXData *data6 = [[NXData alloc] initWithBytes:"foobar" size:6];
    NXString *b64Str6 = [data6 base64String];
    test_assert(b64Str6 != nil);
    test_cstrings_equal([b64Str6 cStr], "Zm9vYmFy");
    [data6 release];

    printf("  ✓ RFC 4648 base64 test vectors pass\n");
  }

  printf("Test 33: base64String - binary data\n");
  {
    char testBytes[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:6];
    NXString *b64Str = [data base64String];
    test_assert(b64Str != nil);
    test_cstrings_equal([b64Str cStr], "AAECAwQF");
    [data release];
    printf("  ✓ base64String for binary data works\n");
  }

  printf("Test 34: base64String - all zero bytes\n");
  {
    char testBytes[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:6];
    NXString *b64Str = [data base64String];
    test_assert(b64Str != nil);
    test_cstrings_equal([b64Str cStr], "AAAAAAAA");
    [data release];
    printf("  ✓ base64String for all zero bytes works\n");
  }

  printf("Test 35: base64String - all 0xFF bytes\n");
  {
    char testBytes[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:6];
    NXString *b64Str = [data base64String];
    test_assert(b64Str != nil);
    test_cstrings_equal([b64Str cStr], "////////");
    [data release];
    printf("  ✓ base64String for all 0xFF bytes works\n");
  }

  printf("Test 36: base64String - edge case byte values\n");
  {
    // Test bytes that exercise different parts of the Base64 alphabet
    char testBytes[] = {0x3E, 0x3F, 0x40}; // Should produce '+', '/', 'A'
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:3];
    NXString *b64Str = [data base64String];
    test_assert(b64Str != nil);
    test_cstrings_equal([b64Str cStr], "Pj9A");
    [data release];
    printf("  ✓ base64String for edge case byte values works\n");
  }

  printf("Test 37: base64String - longer data\n");
  {
    // Test with a longer string to ensure proper handling
    const char *testStr = "The quick brown fox jumps over the lazy dog";
    NXData *data = [[NXData alloc] initWithBytes:testStr size:strlen(testStr)];
    NXString *b64Str = [data base64String];
    test_assert(b64Str != nil);
    test_cstrings_equal(
        [b64Str cStr],
        "VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw==");
    [data release];
    printf("  ✓ base64String for longer data works\n");
  }

  printf("Test 38: base64String - consistency check\n");
  {
    char testBytes[] = {0xDE, 0xAD, 0xBE, 0xEF};
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:4];

    // Call base64String multiple times to ensure consistency
    NXString *b64Str1 = [data base64String];
    NXString *b64Str2 = [data base64String];
    test_assert(b64Str1 != nil);
    test_assert(b64Str2 != nil);
    test_cstrings_equal([b64Str1 cStr], "3q2+7w==");
    test_cstrings_equal([b64Str2 cStr], "3q2+7w==");
    test_cstrings_equal([b64Str1 cStr], [b64Str2 cStr]);

    [data release];
    printf("  ✓ base64String returns consistent results\n");
  }

  printf("Test 39: hexString - large data performance\n");
  {
    // Test with a reasonably large amount of data
    size_t size = 1024;
    char *testBytes = malloc(size);
    for (size_t i = 0; i < size; i++) {
      testBytes[i] = (char)(i % 256);
    }

    NXData *data = [[NXData alloc] initWithBytes:testBytes size:size];
    NXString *hexStr = [data hexString];
    test_assert(hexStr != nil);
    test_assert(strlen([hexStr cStr]) ==
                size * 2); // Each byte becomes 2 hex chars

    free(testBytes);
    [data release];
    printf("  ✓ hexString handles large data correctly\n");
  }

  printf("Test 40: base64String - large data performance\n");
  {
    // Test with a reasonably large amount of data
    size_t size = 1023; // 1023 bytes -> 1364 base64 chars (with padding)
    char *testBytes = malloc(size);
    for (size_t i = 0; i < size; i++) {
      testBytes[i] = (char)(i % 256);
    }

    NXData *data = [[NXData alloc] initWithBytes:testBytes size:size];
    NXString *b64Str = [data base64String];
    test_assert(b64Str != nil);

    // Check expected length: ((size + 2) / 3) * 4
    size_t expectedLen = ((size + 2) / 3) * 4;
    test_assert(strlen([b64Str cStr]) == expectedLen);

    free(testBytes);
    [data release];
    printf("  ✓ base64String handles large data correctly\n");
  }

  printf("Test 41: Cross-format consistency\n");
  {
    // Test that hexString and base64String work with the same data
    char testBytes[] = "Hello, World!";
    size_t size = strlen(testBytes);

    NXData *data = [[NXData alloc] initWithBytes:testBytes size:size];

    NXString *hexStr = [data hexString];
    NXString *b64Str = [data base64String];

    test_assert(hexStr != nil);
    test_assert(b64Str != nil);
    test_cstrings_equal([hexStr cStr], "48656C6C6F2C20576F726C6421");
    test_cstrings_equal([b64Str cStr], "SGVsbG8sIFdvcmxkIQ==");

    [data release];
    printf("  ✓ Both encoding methods work consistently with same data\n");
  }

  printf("Test 42: Edge case - maximum single byte values\n");
  {
    // Test with bytes that represent edge cases in both hex and base64
    char testBytes[] = {0x00, 0x0F, 0x10, 0x1F, 0xF0, 0xFF};
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:6];

    NXString *hexStr = [data hexString];
    NXString *b64Str = [data base64String];

    test_assert(hexStr != nil);
    test_assert(b64Str != nil);
    test_cstrings_equal([hexStr cStr], "000F101FF0FF");
    test_cstrings_equal([b64Str cStr], "AA8QH/D/");

    [data release];
    printf("  ✓ Edge case byte values handled correctly in both formats\n");
  }

  printf("Test 43: String data encoding verification\n");
  {
    // Test encoding of string data created with initWithString
    NXData *data = [[NXData alloc] initWithString:@"ABC"];

    NXString *hexStr = [data hexString];
    NXString *b64Str = [data base64String];

    test_assert(hexStr != nil);
    test_assert(b64Str != nil);
    // "ABC\0" = 0x41, 0x42, 0x43, 0x00
    test_cstrings_equal([hexStr cStr], "41424300");
    test_cstrings_equal([b64Str cStr], "QUJDAA==");

    [data release];
    printf("  ✓ String data encoding works correctly\n");
  }

  printf("All NXData tests completed successfully!\n");
  return 0;
}
