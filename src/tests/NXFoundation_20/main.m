#include <NXFoundation/NXFoundation.h>
#include <stdio.h>
#include <stdlib.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

int test_json_methods(void);

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) {
  NXZone *zone = [NXZone zoneWithSize:2048];
  test_assert(zone != nil);
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];
  test_assert(pool != nil);

  // Run the test for JSON methods
  int returnValue = TestMain("NXFoundation_20", test_json_methods);

  // Clean up
  [pool release];
  [zone release];

  // Return the result of the test
  return returnValue;
}

///////////////////////////////////////////////////////////////////////////////
// TESTS

int test_json_methods(void) {
  printf("\n=== Testing JSONProtocol Conformance ===\n");

  // Test 1: NXString JSONString functionality
  printf("Test 1: Testing NXString JSONString...\n");

  // Basic string
  NXString *basicStr = [NXString stringWithCString:"Hello World"];
  NXString *basicJson = [basicStr JSONString];
  test_assert(basicJson != nil);
  test_cstrings_equal([basicJson cStr], "\"Hello World\"");
  printf("✓ NXString basic JSON string formatting\n");

  // String with quotes
  NXString *quoteStr = [NXString stringWithCString:"Say \"Hello\""];
  NXString *quoteJson = [quoteStr JSONString];
  test_assert(quoteJson != nil);
  test_cstrings_equal([quoteJson cStr], "\"Say \\\"Hello\\\"\"");
  printf("✓ NXString quote escaping in JSON\n");

  // String with control characters
  NXString *controlStr = [NXString stringWithCString:"Line1\nLine2\tTabbed"];
  NXString *controlJson = [controlStr JSONString];
  test_assert(controlJson != nil);
  test_cstrings_equal([controlJson cStr], "\"Line1\\nLine2\\tTabbed\"");
  printf("✓ NXString control character escaping in JSON\n");

  // String with Unicode control characters
  char unicodeStr[6];
  unicodeStr[0] = 'A';
  unicodeStr[1] = '\x01'; // SOH control character
  unicodeStr[2] = '\x1F'; // Unit Separator
  unicodeStr[3] = 'B';
  unicodeStr[4] = '\0';
  NXString *unicodeTestStr = [NXString stringWithCString:unicodeStr];
  NXString *unicodeJson = [unicodeTestStr JSONString];
  test_assert(unicodeJson != nil);
  test_cstrings_equal([unicodeJson cStr], "\"A\\u0001\\u001FB\"");
  printf("✓ NXString Unicode control character escaping in JSON\n");

  // Empty string
  NXString *emptyStr = [NXString stringWithCString:""];
  NXString *emptyJson = [emptyStr JSONString];
  test_assert(emptyJson != nil);
  test_cstrings_equal([emptyJson cStr], "\"\"");
  printf("✓ NXString empty string JSON formatting\n");

  // Test 2: NXNull JSONString functionality
  printf("\nTest 2: Testing NXNull JSONString...\n");

  NXNull *nullValue = [NXNull nullValue];
  NXString *nullJson = [nullValue JSONString];
  test_assert(nullJson != nil);
  test_cstrings_equal([nullJson cStr], "null");
  printf("✓ NXNull JSON string is 'null'\n");

  // Verify it's the same as description
  NXString *nullDesc = [nullValue description];
  test_assert([nullJson isEqual:nullDesc]);
  printf("✓ NXNull JSONString matches description\n");

  // Test 3: NXNumber JSONString functionality
  printf("\nTest 3: Testing NXNumber JSONString...\n");

  // Boolean values
  NXNumber *boolTrue = [NXNumber numberWithBool:YES];
  NXString *boolTrueJson = [boolTrue JSONString];
  test_assert(boolTrueJson != nil);
  test_cstrings_equal([boolTrueJson cStr], "true");
  printf("✓ NXNumber boolean true JSON string\n");

  NXNumber *boolFalse = [NXNumber numberWithBool:NO];
  NXString *boolFalseJson = [boolFalse JSONString];
  test_assert(boolFalseJson != nil);
  test_cstrings_equal([boolFalseJson cStr], "false");
  printf("✓ NXNumber boolean false JSON string\n");

  // Integer values - using zero value which works correctly
  NXNumber *zeroNum = [NXNumber zeroValue];
  NXString *zeroJson = [zeroNum JSONString];
  test_assert(zeroJson != nil);
  test_cstrings_equal([zeroJson cStr], "0");
  printf("✓ NXNumber zero JSON string\n");

  // Positive integer values
  NXNumber *intNum = [NXNumber numberWithInt32:42];
  NXString *intJson = [intNum JSONString];
  test_assert(intJson != nil);
  test_cstrings_equal([intJson cStr], "42");
  printf("✓ NXNumber positive integer JSON string\n");

  // Negative numbers
  NXNumber *negIntNum = [NXNumber numberWithInt32:-123];
  NXString *negIntJson = [negIntNum JSONString];
  test_assert(negIntJson != nil);
  test_cstrings_equal([negIntJson cStr], "-123");
  printf("✓ NXNumber negative integer JSON string\n");

  // Large positive numbers
  NXNumber *largeNum =
      [NXNumber numberWithInt64:9223372036854775807LL]; // MAX_INT64
  NXString *largeJson = [largeNum JSONString];
  test_assert(largeJson != nil);
  test_cstrings_equal([largeJson cStr], "9223372036854775807");
  printf("✓ NXNumber large integer JSON string\n");

  // Large negative numbers
  NXNumber *largeNegNum =
      [NXNumber numberWithInt64:-9223372036854775807LL]; // MIN_INT64 + 1
  NXString *largeNegJson = [largeNegNum JSONString];
  test_assert(largeNegJson != nil);
  test_cstrings_equal([largeNegJson cStr], "-9223372036854775807");
  printf("✓ NXNumber large negative integer JSON string\n");

  // Unsigned numbers
  NXNumber *unsignedNum =
      [NXNumber numberWithUnsignedInt32:4294967295U]; // MAX_UINT32
  NXString *unsignedJson = [unsignedNum JSONString];
  test_assert(unsignedJson != nil);
  test_cstrings_equal([unsignedJson cStr], "4294967295");
  printf("✓ NXNumber unsigned integer JSON string\n");

  // Small numbers
  NXNumber *oneNum = [NXNumber numberWithInt32:1];
  NXString *oneJson = [oneNum JSONString];
  test_assert(oneJson != nil);
  test_cstrings_equal([oneJson cStr], "1");
  printf("✓ NXNumber single digit JSON string\n");

  // Test specific values that commonly cause issues
  NXNumber *tenNum = [NXNumber numberWithInt32:10];
  NXString *tenJson = [tenNum JSONString];
  test_assert(tenJson != nil);
  test_cstrings_equal([tenJson cStr], "10");
  printf("✓ NXNumber two digit JSON string\n");

  NXNumber *hundredNum = [NXNumber numberWithInt32:100];
  NXString *hundredJson = [hundredNum JSONString];
  test_assert(hundredJson != nil);
  test_cstrings_equal([hundredJson cStr], "100");
  printf("✓ NXNumber three digit JSON string\n");

  // Test 4: NXDate JSONString functionality
  printf("\nTest 4: Testing NXDate JSONString...\n");

  // Current date
  NXDate *currentDate = [NXDate date];
  NXString *dateJson = [currentDate JSONString];
  test_assert(dateJson != nil);

  // Verify JSONString is quoted version of description
  NXString *dateDesc = [currentDate description];

  // Build expected JSON manually since we can't use @ format specifier
  NXString *expectedJson = [NXString stringWithCString:"\""];
  [expectedJson append:dateDesc];
  [expectedJson appendCString:"\""];

  test_assert([dateJson isEqual:expectedJson]);
  printf("✓ NXDate JSONString is quoted version of description\n");

  // Verify ISO 8601 format pattern ("YYYY-MM-DDTHH:MM:SSZ")
  const char *dateStr = [dateJson cStr];
  test_assert(strlen(dateStr) ==
              22); // Should be exactly 22 characters (including quotes)
  test_assert(dateStr[0] == '"');  // Should start with quote
  test_assert(dateStr[5] == '-');  // Year-Month separator
  test_assert(dateStr[8] == '-');  // Month-Day separator
  test_assert(dateStr[11] == 'T'); // Date-Time separator
  test_assert(dateStr[14] == ':'); // Hour-Minute separator
  test_assert(dateStr[17] == ':'); // Minute-Second separator
  test_assert(dateStr[20] == 'Z'); // UTC timezone indicator
  test_assert(dateStr[21] == '"'); // Should end with quote
  printf("✓ NXDate JSON string follows quoted ISO 8601 format\n");

  // Test date with specific time
  NXDate *specificDate =
      [NXDate dateWithTimeIntervalSinceNow:-3600]; // 1 hour ago
  NXString *specificJson = [specificDate JSONString];
  test_assert(specificJson != nil);
  // Just verify it has the correct ISO format structure, not exact content
  const char *specificDateStr = [specificJson cStr];
  test_assert(strlen(specificDateStr) ==
              22); // Should be exactly 22 characters (including quotes)
  test_assert(specificDateStr[0] == '"');  // Should start with quote
  test_assert(specificDateStr[5] == '-');  // Year-Month separator
  test_assert(specificDateStr[8] == '-');  // Month-Day separator
  test_assert(specificDateStr[11] == 'T'); // Date-Time separator
  test_assert(specificDateStr[14] == ':'); // Hour-Minute separator
  test_assert(specificDateStr[17] == ':'); // Minute-Second separator
  test_assert(specificDateStr[20] == 'Z'); // UTC timezone indicator
  test_assert(specificDateStr[21] == '"'); // Should end with quote
  printf("✓ NXDate time interval JSON formatting\n");

  // Test 5: JSON compliance validation
  printf("\nTest 5: Testing JSON specification compliance...\n");

  // Test that all JSON strings are valid
  // NXString should always produce quoted strings
  NXString *testStr = [NXString stringWithCString:"test"];
  NXString *testStrJson = [testStr JSONString];
  const char *testStrJsonCStr = [testStrJson cStr];
  test_assert(testStrJsonCStr[0] == '"');
  test_assert(testStrJsonCStr[[testStrJson length] - 1] == '"');
  printf("✓ NXString JSON output is properly quoted\n");

  // NXNull should produce literal null
  NXNull *testNull = [NXNull nullValue];
  NXString *testNullJson = [testNull JSONString];
  test_cstrings_equal([testNullJson cStr], "null");
  printf("✓ NXNull JSON output is literal 'null'\n");

  // NXNumber booleans should produce literal true/false
  NXNumber *testTrue = [NXNumber numberWithBool:YES];
  NXNumber *testFalse = [NXNumber numberWithBool:NO];
  test_cstrings_equal([[testTrue JSONString] cStr], "true");
  test_cstrings_equal([[testFalse JSONString] cStr], "false");
  printf("✓ NXNumber boolean JSON output is literal true/false\n");

  // NXNumber integers should produce unquoted numbers
  NXNumber *testInt = [NXNumber numberWithInt32:123];
  NXString *testIntJson = [testInt JSONString];
  const char *intJsonStr = [testIntJson cStr];
  test_assert(intJsonStr[0] != '"'); // Should not be quoted
  test_assert(intJsonStr[strlen(intJsonStr) - 1] !=
              '"');                       // Should not be quoted
  test_cstrings_equal(intJsonStr, "123"); // Verify exact content
  printf("✓ NXNumber integer JSON output is unquoted and correct\n");

  // Test negative numbers are properly formatted
  NXNumber *testNegInt = [NXNumber numberWithInt32:-456];
  NXString *testNegIntJson = [testNegInt JSONString];
  const char *negIntJsonStr = [testNegIntJson cStr];
  test_assert(negIntJsonStr[0] == '-'); // Should start with minus sign
  test_assert(negIntJsonStr[1] != '"'); // Should not be quoted after minus
  test_cstrings_equal(negIntJsonStr, "-456"); // Verify exact content
  printf("✓ NXNumber negative integer JSON output is correct\n");

  // NXDate should produce quoted ISO 8601 strings
  NXDate *testDate = [NXDate date];
  NXString *testDateJson = [testDate JSONString];
  const char *dateJsonStr = [testDateJson cStr];
  test_assert(dateJsonStr[0] == '"'); // Should start with quote
  test_assert(dateJsonStr[strlen(dateJsonStr) - 1] ==
              '"'); // Should end with quote
  test_assert(strlen(dateJsonStr) ==
              22); // Should be exactly 22 characters including quotes
  printf("✓ NXDate JSON output format verified\n");

  // Test 6: Protocol conformance verification
  printf("\nTest 6: Testing protocol conformance...\n");

  // Verify all classes actually conform to JSONProtocol
  test_assert([NXString conformsTo:@protocol(JSONProtocol)]);
  printf("✓ NXString conforms to JSONProtocol\n");

  test_assert([NXNull conformsTo:@protocol(JSONProtocol)]);
  printf("✓ NXNull conforms to JSONProtocol\n");

  test_assert([NXNumber conformsTo:@protocol(JSONProtocol)]);
  printf("✓ NXNumber conforms to JSONProtocol\n");

  test_assert([NXDate conformsTo:@protocol(JSONProtocol)]);
  printf("✓ NXDate conforms to JSONProtocol\n");

  // Test 7: Edge cases and error conditions
  printf("\nTest 7: Testing edge cases...\n");

  // Test that JSONString never returns nil
  NXString *emptyTestStr = [NXString stringWithCString:""];
  test_assert([emptyTestStr JSONString] != nil);
  printf("✓ Empty string JSONString returns non-nil\n");

  NXNumber *zeroTestNum = [NXNumber numberWithInt32:0];
  test_assert([zeroTestNum JSONString] != nil);
  printf("✓ Zero number JSONString returns non-nil\n");

  test_assert([[NXNull nullValue] JSONString] != nil);
  printf("✓ Null value JSONString returns non-nil\n");

  // Test JSON string lengths are reasonable
  test_assert([[emptyTestStr JSONString] length] >=
              2); // At minimum should be ""
  test_assert([[zeroTestNum JSONString] length] >=
              1); // At minimum should be "0"
  test_assert([[[NXNull nullValue] JSONString] length] ==
              4); // Should be exactly "null"
  printf("✓ JSON string lengths are reasonable\n");

  printf("\nTest 6: Testing NXData JSONString and JSONBytes...\n");

  // Test empty data
  NXData *emptyData = [[NXData alloc] init];
  test_assert([emptyData JSONBytes] == 1); // Empty Base64: 1 (null terminator)
  NXString *emptyDataJson = [emptyData JSONString];
  test_assert(emptyDataJson != nil);
  test_cstrings_equal([emptyDataJson cStr],
                      ""); // Empty data -> empty Base64 string
  [emptyData release];
  printf("✓ Empty NXData JSON serialization\n");

  // Test single byte data
  char singleByte = 'A'; // 0x41
  NXData *singleData = [[NXData alloc] initWithBytes:&singleByte size:1];
  test_assert([singleData JSONBytes] ==
              5); // "QQ==" (4 chars + null terminator)
  NXString *singleJson = [singleData JSONString];
  test_assert(singleJson != nil);
  test_cstrings_equal([singleJson cStr], "QQ==");
  [singleData release];
  printf("✓ Single byte NXData JSON serialization\n");

  // Test string data (with null terminator)
  NXData *stringData = [[NXData alloc] initWithString:@"Test"];
  test_assert([stringData JSONBytes] ==
              9); // "VGVzdAA=" (8 chars + null terminator)
  NXString *stringJson = [stringData JSONString];
  test_assert(stringJson != nil);
  test_cstrings_equal([stringJson cStr], "VGVzdAA="); // "Test\0" in Base64
  [stringData release];
  printf("✓ String-based NXData JSON serialization\n");

  // Test binary data
  char binaryBytes[] = {0x00, 0x01, 0x02, 0x03, 0xFF, 0xFE};
  NXData *binaryData = [[NXData alloc] initWithBytes:binaryBytes size:6];
  test_assert([binaryData JSONBytes] ==
              9); // "AAECA//+" (8 chars + null terminator)
  NXString *binaryJson = [binaryData JSONString];
  test_assert(binaryJson != nil);
  test_cstrings_equal([binaryJson cStr], "AAECA//+");
  [binaryData release];
  printf("✓ Binary NXData JSON serialization\n");

  // Test RFC 4648 compliance
  NXData *rfcData = [[NXData alloc] initWithBytes:"foobar" size:6];
  test_assert([rfcData JSONBytes] ==
              9); // "Zm9vYmFy" (8 chars + null terminator)
  NXString *rfcJson = [rfcData JSONString];
  test_assert(rfcJson != nil);
  test_cstrings_equal([rfcJson cStr], "Zm9vYmFy"); // RFC 4648 test vector
  [rfcData release];
  printf("✓ NXData RFC 4648 Base64 compliance\n");

  // Test consistency between JSONBytes and JSONString
  char testBytes[] = {0xDE, 0xAD, 0xBE, 0xEF};
  NXData *consistencyData = [[NXData alloc] initWithBytes:testBytes size:4];
  size_t estimatedBytes = [consistencyData JSONBytes];
  NXString *actualJson = [consistencyData JSONString];
  test_assert(actualJson != nil);
  size_t actualLength = strlen([actualJson cStr]) + 1; // +1 for null terminator
  test_assert(estimatedBytes == actualLength);
  test_cstrings_equal([actualJson cStr], "3q2+7w==");
  [consistencyData release];
  printf("✓ JSONBytes estimation matches actual JSONString length\n");

  // Test larger data for performance validation
  size_t largeSize = 100;
  char *largeBytes = malloc(largeSize);
  for (size_t i = 0; i < largeSize; i++) {
    largeBytes[i] = (char)(i % 256);
  }
  NXData *largeData = [[NXData alloc] initWithBytes:largeBytes size:largeSize];
  size_t largeEstimate = [largeData JSONBytes];
  NXString *largeDataJson = [largeData JSONString];
  test_assert(largeDataJson != nil);
  test_assert(largeEstimate == strlen([largeDataJson cStr]) + 1);
  free(largeBytes);
  [largeData release];
  printf("✓ Large NXData JSON serialization performance\n");

  printf("\n✅ All JSONProtocol conformance tests passed!\n");
  return 0;
}
