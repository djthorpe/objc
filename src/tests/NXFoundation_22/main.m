#include <NXFoundation/NXFoundation.h>
#include <stdio.h>
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
    test_assert([data size] == 8); // 6 chars + null terminator = 7, but
                                   // Test\n\t\\ is 7 chars + null = 8

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

  printf("Test 16: JSONBytes method\n");
  {
    NXData *data1 = [[NXData alloc] initWithCapacity:10];
    test_assert([data1 JSONBytes] == 0); // size * 2 = 0 * 2 = 0
    [data1 release];

    NXData *data2 = [[NXData alloc] initWithString:@"Test"];
    test_assert([data2 JSONBytes] == 10); // size * 2 = 5 * 2 = 10
    [data2 release];
    printf("  ✓ JSONBytes method works\n");
  }

  printf("Test 17: Accessor methods consistency\n");
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

  printf("Test 18: String with Unicode-like content\n");
  {
    NXData *data = [[NXData alloc] initWithString:@"café"];
    test_assert(data != nil);
    test_assert([data size] > 0);
    test_assert([data bytes] != NULL);
    [data release];
    printf("  ✓ Unicode-like string content works\n");
  }

  printf("Test 19: Maximum reasonable capacity\n");
  {
    // Test a reasonably large but not excessive capacity
    NXData *data = [[NXData alloc] initWithCapacity:1024 * 1024]; // 1MB
    test_assert(data != nil);
    test_assert([data capacity] == 1024 * 1024);
    test_assert([data size] == 0);
    [data release];
    printf("  ✓ Large capacity (1MB) allocation works\n");
  }

  printf("Test 20: Edge case - single byte\n");
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

  printf("Test 21: hexString - empty data\n");
  {
    NXData *data = [[NXData alloc] init];
    NXString *hexStr = [data hexString];
    test_assert(hexStr != nil);
    test_cstrings_equal([hexStr cStr], "");
    [data release];
    printf("  ✓ hexString for empty data returns empty string\n");
  }

  printf("Test 22: hexString - single byte\n");
  {
    char singleByte = 0xAB;
    NXData *data = [[NXData alloc] initWithBytes:&singleByte size:1];
    NXString *hexStr = [data hexString];
    test_assert(hexStr != nil);
    test_cstrings_equal([hexStr cStr], "AB");
    [data release];
    printf("  ✓ hexString for single byte works\n");
  }

  printf("Test 23: hexString - multiple bytes\n");
  {
    char testBytes[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:8];
    NXString *hexStr = [data hexString];
    test_assert(hexStr != nil);
    test_cstrings_equal([hexStr cStr], "0123456789ABCDEF");
    [data release];
    printf("  ✓ hexString for multiple bytes works\n");
  }

  printf("Test 24: hexString - zero bytes\n");
  {
    char testBytes[] = {0x00, 0x00, 0x00};
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:3];
    NXString *hexStr = [data hexString];
    test_assert(hexStr != nil);
    test_cstrings_equal([hexStr cStr], "000000");
    [data release];
    printf("  ✓ hexString for zero bytes works\n");
  }

  printf("Test 25: hexString - all possible byte values\n");
  {
    char testBytes[] = {0x00, 0x0F, 0xF0, 0xFF};
    NXData *data = [[NXData alloc] initWithBytes:testBytes size:4];
    NXString *hexStr = [data hexString];
    test_assert(hexStr != nil);
    test_cstrings_equal([hexStr cStr], "000FF0FF");
    [data release];
    printf("  ✓ hexString for edge byte values works\n");
  }

  printf("Test 26: hexString - string data\n");
  {
    NXData *data = [[NXData alloc] initWithString:@"Hi"];
    NXString *hexStr = [data hexString];
    test_assert(hexStr != nil);
    // "Hi" + null terminator = 0x48, 0x69, 0x00
    test_cstrings_equal([hexStr cStr], "486900");
    [data release];
    printf("  ✓ hexString for string data works\n");
  }

  printf("Test 27: hexString - consistency check\n");
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

  printf("All NXData tests completed successfully!\n");
  return 0;
}
