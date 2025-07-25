#include <NXFoundation/NXFoundation.h>
#include <stdio.h>
#include <string.h>
#include <tests/tests.h>

// Forward declaration
int test_nxstring_methods(void);

int main() {
  NXZone *zone = [NXZone zoneWithSize:2048];
  test_assert(zone != nil);
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];
  test_assert(pool != nil);

  // Run the test for NXString methods
  int returnValue = TestMain("NXFoundation_18", test_nxstring_methods);

  // Clean up
  [pool release];
  [zone release];

  // Return the result of the test
  return returnValue;
}

int test_nxstring_methods(void) {
  printf("\nTest 1: Testing basic string creation methods...\n");

  // Test stringWithCString
  NXString *str1 = [NXString stringWithCString:"Hello World"];
  test_assert(str1 != nil);
  test_assert([str1 length] == 11);
  test_cstrings_equal([str1 cStr], "Hello World");
  printf("✓ stringWithCString works correctly\n");

  // Test initWithCString
  NXString *str2 = [[NXString alloc] initWithCString:"Test String"];
  test_assert(str2 != nil);
  test_assert([str2 length] == 11);
  test_cstrings_equal([str2 cStr], "Test String");
  printf("✓ initWithCString works correctly\n");

  // Test stringWithString
  NXString *str3 = [NXString stringWithString:str1];
  test_assert(str3 != nil);
  test_assert([str3 length] == [str1 length]);
  test_cstrings_equal([str3 cStr], [str1 cStr]);
  printf("✓ stringWithString works correctly\n");

  // Test initWithString
  NXString *str4 = [[NXString alloc] initWithString:str2];
  test_assert(str4 != nil);
  test_assert([str4 length] == [str2 length]);
  test_cstrings_equal([str4 cStr], [str2 cStr]);
  printf("✓ initWithString works correctly\n");

  printf("\nTest 2: Testing string formatting methods...\n");

  // Test stringWithFormat
  NXString *formatted1 =
      [NXString stringWithFormat:@"Number: %d, String: %s", 42, "test"];
  test_assert(formatted1 != nil);
  test_assert([formatted1 length] > 0);
  test_cstrings_equal([formatted1 cStr], "Number: 42, String: test");
  printf("✓ stringWithFormat works correctly\n");

  // Test initWithFormat
  NXString *formatted2 =
      [[NXString alloc] initWithFormat:@"Value: %s", "example"];
  test_assert(formatted2 != nil);
  test_assert([formatted2 length] > 0);
  test_cstrings_equal([formatted2 cStr], "Value: example");
  printf("✓ initWithFormat works correctly\n");

  printf("\nTest 3: Testing string comparison methods...\n");

  // Test isEqual with identical strings
  NXString *str5 = [NXString stringWithCString:"Hello"];
  NXString *str6 = [NXString stringWithCString:"Hello"];
  test_assert([str5 isEqual:str6] == YES);
  printf("✓ isEqual works for identical strings\n");

  // Test isEqual with different strings
  NXString *str7 = [NXString stringWithCString:"Hello"];
  NXString *str8 = [NXString stringWithCString:"World"];
  test_assert([str7 isEqual:str8] == NO);
  printf("✓ isEqual works for different strings\n");

  // Test isEqual with nil
  test_assert([str7 isEqual:nil] == NO);
  printf("✓ isEqual works with nil\n");

  // Test compare method
  NXString *strA = [NXString stringWithCString:"apple"];
  NXString *strB = [NXString stringWithCString:"banana"];
  NXString *strC = [NXString stringWithCString:"apple"];

  test_assert([strA compare:strB] < 0);  // apple < banana
  test_assert([strB compare:strA] > 0);  // banana > apple
  test_assert([strA compare:strC] == 0); // apple == apple
  printf("✓ compare method works correctly\n");

  printf("\nTest 4: Testing character counting methods...\n");

  // Test countOccurrencesOfByte
  NXString *testStr = [NXString stringWithCString:"Hello World"];
  test_assert([testStr countOccurrencesOfByte:'l'] == 3);
  test_assert([testStr countOccurrencesOfByte:'o'] == 2);
  test_assert([testStr countOccurrencesOfByte:'x'] == 0);
  printf("✓ countOccurrencesOfByte works correctly\n");

  printf("\nTest 5: Testing empty and new string methods...\n");

  // Test new method
  NXString *emptyStr = [NXString new];
  test_assert(emptyStr != nil);
  test_assert([emptyStr length] == 0);
  test_cstrings_equal([emptyStr cStr], "");
  printf("✓ new method works correctly\n");

  // Test init method
  NXString *emptyStr2 = [[NXString alloc] init];
  test_assert(emptyStr2 != nil);
  test_assert([emptyStr2 length] == 0);
  test_cstrings_equal([emptyStr2 cStr], "");
  printf("✓ init method works correctly\n");

  printf("\nTest 6: Testing description method...\n");

  NXString *descStr = [NXString stringWithCString:"Test Description"];
  NXString *desc = [descStr description];
  test_assert(desc == descStr); // description should return self
  printf("✓ description method works correctly\n");

  printf("\nTest 7: Testing hasPrefix and hasSuffix methods...\n");

  // Test hasPrefix with various cases
  NXString *testPrefix = [NXString stringWithCString:"Hello World"];

  // Test hasPrefix - normal cases
  test_assert([testPrefix hasPrefix:[NXString stringWithCString:"Hello"]] ==
              YES);
  test_assert([testPrefix hasPrefix:[NXString stringWithCString:"Hell"]] ==
              YES);
  test_assert([testPrefix hasPrefix:[NXString stringWithCString:"H"]] == YES);
  test_assert(
      [testPrefix hasPrefix:[NXString stringWithCString:"Hello World"]] ==
      YES); // Exact match
  printf("✓ hasPrefix works for valid prefixes\n");

  // Test hasPrefix - negative cases
  test_assert([testPrefix hasPrefix:[NXString stringWithCString:"World"]] ==
              NO);
  test_assert([testPrefix hasPrefix:[NXString stringWithCString:"hello"]] ==
              NO); // Case sensitive
  test_assert(
      [testPrefix hasPrefix:[NXString stringWithCString:"Hello World!"]] ==
      NO); // Longer than string
  printf("✓ hasPrefix correctly rejects invalid prefixes\n");

  // Test hasPrefix - edge cases
  test_assert([testPrefix hasPrefix:[NXString stringWithCString:""]] ==
              YES); // Empty prefix
  NXString *emptyTestStr = [NXString stringWithCString:""];
  test_assert([emptyTestStr hasPrefix:[NXString stringWithCString:""]] ==
              YES); // Empty string, empty prefix
  test_assert([emptyTestStr hasPrefix:[NXString stringWithCString:"a"]] ==
              NO); // Empty string, non-empty prefix
  printf("✓ hasPrefix handles edge cases correctly\n");

  // Test hasSuffix with various cases
  NXString *testSuffix = [NXString stringWithCString:"Hello World"];

  // Test hasSuffix - normal cases
  test_assert([testSuffix hasSuffix:[NXString stringWithCString:"World"]] ==
              YES);
  test_assert([testSuffix hasSuffix:[NXString stringWithCString:"orld"]] ==
              YES);
  test_assert([testSuffix hasSuffix:[NXString stringWithCString:"d"]] == YES);
  test_assert(
      [testSuffix hasSuffix:[NXString stringWithCString:"Hello World"]] ==
      YES); // Exact match
  printf("✓ hasSuffix works for valid suffixes\n");

  // Test hasSuffix - negative cases
  test_assert([testSuffix hasSuffix:[NXString stringWithCString:"Hello"]] ==
              NO);
  test_assert([testSuffix hasSuffix:[NXString stringWithCString:"world"]] ==
              NO); // Case sensitive
  test_assert(
      [testSuffix hasSuffix:[NXString stringWithCString:"!Hello World"]] ==
      NO); // Longer than string
  printf("✓ hasSuffix correctly rejects invalid suffixes\n");

  // Test hasSuffix - edge cases
  test_assert([testSuffix hasSuffix:[NXString stringWithCString:""]] ==
              YES); // Empty suffix
  test_assert([emptyTestStr hasSuffix:[NXString stringWithCString:""]] ==
              YES); // Empty string, empty suffix
  test_assert([emptyTestStr hasSuffix:[NXString stringWithCString:"a"]] ==
              NO); // Empty string, non-empty suffix
  printf("✓ hasSuffix handles edge cases correctly\n");

  // Test prefix/suffix with single character strings
  NXString *singleChar = [NXString stringWithCString:"A"];
  test_assert([singleChar hasPrefix:[NXString stringWithCString:"A"]] == YES);
  test_assert([singleChar hasSuffix:[NXString stringWithCString:"A"]] == YES);
  test_assert([singleChar hasPrefix:[NXString stringWithCString:"B"]] == NO);
  test_assert([singleChar hasSuffix:[NXString stringWithCString:"B"]] == NO);
  printf(
      "✓ hasPrefix/hasSuffix work correctly with single character strings\n");

  // Test prefix/suffix with multi-word strings
  NXString *multiWord = [NXString stringWithCString:"The quick brown fox"];
  test_assert([multiWord hasPrefix:[NXString stringWithCString:"The quick"]] ==
              YES);
  test_assert([multiWord hasSuffix:[NXString stringWithCString:"brown fox"]] ==
              YES);
  test_assert(
      [multiWord hasPrefix:[NXString stringWithCString:"quick brown"]] == NO);
  test_assert([multiWord hasSuffix:[NXString stringWithCString:"The quick"]] ==
              NO);
  printf("✓ hasPrefix/hasSuffix work correctly with multi-word strings\n");

  printf("\nTest 8: Testing toUppercase and toLowercase methods...\n");

  // Test toUppercase with formatted strings (safe to modify)
  NXString *upperTest1 = [NXString stringWithFormat:@"hello world"];
  [upperTest1 toUppercase];
  test_cstrings_equal([upperTest1 cStr], "HELLO WORLD");
  printf("✓ toUppercase works with basic lowercase text\n");

  // Test toLowercase with formatted strings (safe to modify)
  NXString *lowerTest1 = [NXString stringWithFormat:@"HELLO WORLD"];
  [lowerTest1 toLowercase];
  test_cstrings_equal([lowerTest1 cStr], "hello world");
  printf("✓ toLowercase works with basic uppercase text\n");

  // Test mixed case conversion
  NXString *mixedTest = [NXString stringWithFormat:@"HeLLo WoRLd"];
  [mixedTest toUppercase];
  test_cstrings_equal([mixedTest cStr], "HELLO WORLD");
  [mixedTest toLowercase];
  test_cstrings_equal([mixedTest cStr], "hello world");
  printf("✓ toUppercase/toLowercase work with mixed case\n");

  // Test with numbers and special characters (should remain unchanged)
  NXString *specialTest = [NXString stringWithFormat:@"Hello123!@# World"];
  [specialTest toUppercase];
  test_cstrings_equal([specialTest cStr], "HELLO123!@# WORLD");
  [specialTest toLowercase];
  test_cstrings_equal([specialTest cStr], "hello123!@# world");
  printf("✓ toUppercase/toLowercase preserve numbers and special characters\n");

  // Test with single character
  NXString *singleTest = [NXString stringWithFormat:@"a"];
  [singleTest toUppercase];
  test_cstrings_equal([singleTest cStr], "A");
  [singleTest toLowercase];
  test_cstrings_equal([singleTest cStr], "a");
  printf("✓ toUppercase/toLowercase work with single characters\n");

  // Test with already uppercase/lowercase strings
  NXString *alreadyUpper = [NXString stringWithFormat:@"ALREADY UPPER"];
  [alreadyUpper toUppercase];
  test_cstrings_equal([alreadyUpper cStr], "ALREADY UPPER");
  printf("✓ toUppercase handles already uppercase strings\n");

  NXString *alreadyLower = [NXString stringWithFormat:@"already lower"];
  [alreadyLower toLowercase];
  test_cstrings_equal([alreadyLower cStr], "already lower");
  printf("✓ toLowercase handles already lowercase strings\n");

  // Test with empty string
  NXString *emptyCase = [NXString stringWithFormat:@""];
  [emptyCase toUppercase];
  test_cstrings_equal([emptyCase cStr], "");
  [emptyCase toLowercase];
  test_cstrings_equal([emptyCase cStr], "");
  printf("✓ toUppercase/toLowercase handle empty strings\n");

  // Test with only special characters
  NXString *onlySpecial = [NXString stringWithFormat:@"!@#$^&*()"];
  [onlySpecial toUppercase];
  test_cstrings_equal([onlySpecial cStr], "!@#$^&*()");
  [onlySpecial toLowercase];
  test_cstrings_equal([onlySpecial cStr], "!@#$^&*()");
  printf("✓ toUppercase/toLowercase handle special character strings\n");

  // Test with NULL value (should not crash)
  NXString *nullTest =
      [[NXString alloc] init]; // Creates string with NULL _value
  [nullTest toUppercase];      // Should return safely without crash
  [nullTest toLowercase];      // Should return safely without crash
  test_assert([nullTest length] == 0);
  printf("✓ toUppercase/toLowercase handle NULL values safely\n");

  // Test converting referenced (immutable) strings to mutable
  NXString *referencedStr = [NXString stringWithCString:"hello world"];
  [referencedStr toUppercase]; // Should make it mutable and convert
  test_cstrings_equal([referencedStr cStr], "HELLO WORLD");
  [referencedStr toLowercase]; // Should work since it's now mutable
  test_cstrings_equal([referencedStr cStr], "hello world");
  printf(
      "✓ toUppercase/toLowercase can convert referenced strings to mutable\n");

  printf("\nTest 9: Testing memory management...\n");

  // Test autoreleased strings don't crash
  for (int i = 0; i < 5; i++) {
    NXString *tempStr = [NXString stringWithCString:"Temporary"];
    test_assert(tempStr != nil);
    test_assert([tempStr length] == 9);
  }
  printf("✓ Memory management works correctly\n");

  printf("\nTest 10: Testing edge cases and bug fixes...\n");

  // Test initWithCString with NULL input
  NXString *nullCStr = [[NXString alloc] initWithCString:NULL];
  test_assert(nullCStr != nil);
  test_assert([nullCStr length] == 0);
  test_cstrings_equal([nullCStr cStr], "");
  printf("✓ initWithCString handles NULL input correctly\n");
  [nullCStr release];

  // Test stringWithCString with NULL input (through class method)
  NXString *nullClassStr = [NXString stringWithCString:NULL];
  test_assert(nullClassStr != nil);
  test_assert([nullClassStr length] == 0);
  test_cstrings_equal([nullClassStr cStr], "");
  printf("✓ stringWithCString handles NULL input correctly\n");

  // Test initWithCapacity with zero capacity
  NXString *zeroCapStr = [[NXString alloc] initWithCapacity:0];
  test_assert(zeroCapStr != nil);
  test_assert([zeroCapStr length] == 0);
  test_assert([zeroCapStr capacity] == 0);
  printf("✓ initWithCapacity(0) works correctly\n");
  [zeroCapStr release];

  // Test initWithCapacity with non-zero capacity
  NXString *capStr = [[NXString alloc] initWithCapacity:50];
  test_assert(capStr != nil);
  test_assert([capStr length] == 0);
  test_assert([capStr capacity] == 50);
  printf("✓ initWithCapacity(50) works correctly\n");
  [capStr release];

  // Test stringWithCapacity (class method)
  NXString *classCapStr = [NXString stringWithCapacity:25];
  test_assert(classCapStr != nil);
  test_assert([classCapStr length] == 0);
  test_assert([classCapStr capacity] == 25);
  printf("✓ stringWithCapacity(25) works correctly\n");

  // Test stringWithFormat with empty format
  NXString *emptyFormatStr = [NXString stringWithFormat:@""];
  test_assert(emptyFormatStr != nil);
  test_assert([emptyFormatStr length] == 0);
  test_cstrings_equal([emptyFormatStr cStr], "");
  printf("✓ stringWithFormat with empty string works correctly\n");

  // Test initWithFormat with empty format
  NXString *emptyInitStr = [[NXString alloc] initWithFormat:@""];
  test_assert(emptyInitStr != nil);
  test_assert([emptyInitStr length] == 0);
  test_cstrings_equal([emptyInitStr cStr], "");
  printf("✓ initWithFormat with empty string works correctly\n");
  [emptyInitStr release];

  // Clean up manually allocated strings
  [str2 release];
  [str4 release];
  [formatted2 release];
  [emptyStr2 release];
  [nullTest release];

  printf("\n✅ All NXString method tests passed (including "
         "hasPrefix/hasSuffix, toUppercase/toLowercase, and edge cases)!\n");
  return 0;
}
