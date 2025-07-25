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

  printf("\nTest 11: Extended NXString tests...\n");

  // Test isEqual with different string types
  NXString *s1 = [NXString stringWithCString:"test"];
  NXConstantString *s2 = @"test";
  test_assert([s1 isEqual:s2] == YES);
  printf("✓ isEqual with NXConstantString works correctly\n");

  // Test with a non-string object
  NXObject *obj = [[NXObject alloc] init];
  test_assert([s1 isEqual:obj] == NO);
  printf("✓ isEqual with non-string object works correctly\n");
  [obj release];

  // Test countOccurrencesOfByte with empty string
  NXString *empty = [NXString new];
  test_assert([empty countOccurrencesOfByte:'a'] == 0);
  printf("✓ countOccurrencesOfByte with empty string works correctly\n");

  // Test hasPrefix/hasSuffix with NXConstantString
  NXString *prefixStr = [NXString stringWithCString:"prefix_test"];
  test_assert([prefixStr hasPrefix:@"prefix"] == YES);
  test_assert([prefixStr hasSuffix:@"_test"] == YES);
  printf("✓ hasPrefix/hasSuffix with NXConstantString works correctly\n");

  // Test case conversions on immutable strings
  NXString *immutableUpper = [NXString stringWithCString:"immutable"];
  [immutableUpper toUppercase];
  test_cstrings_equal([immutableUpper cStr], "IMMUTABLE");
  printf("✓ toUppercase on immutable string works correctly\n");

  NXString *immutableLower = [NXString stringWithCString:"IMMUTABLE"];
  [immutableLower toLowercase];
  test_cstrings_equal([immutableLower cStr], "immutable");
  printf("✓ toLowercase on immutable string works correctly\n");

  // Test initWithString with NXConstantString
  NXString *initWithConst = [[NXString alloc] initWithString:@"constant"];
  test_cstrings_equal([initWithConst cStr], "constant");
  printf("✓ initWithString with NXConstantString works correctly\n");
  [initWithConst release];

  // Test length and cStr on empty strings
  NXString *emptyInit = [[NXString alloc] init];
  test_assert([emptyInit length] == 0);
  test_cstrings_equal([emptyInit cStr], "");
  printf("✓ length and cStr on empty init string work correctly\n");
  [emptyInit release];

  // Test capacity
  NXString *capTest = [NXString stringWithCapacity:100];
  test_assert([capTest capacity] >= 100);
  printf("✓ capacity is reported correctly\n");

  printf("\n✅ All extended NXString tests passed!\n");

  printf("\nTest 12: Testing append: method...\n");

  // Test basic append with string objects
  NXString *appendBase1 = [NXString stringWithFormat:@"Hello"];
  NXString *appendOther1 = [NXString stringWithCString:" World"];
  BOOL appendResult1 = [appendBase1 append:appendOther1];
  test_assert(appendResult1 == YES);
  test_assert([appendBase1 length] == 11);
  test_cstrings_equal([appendBase1 cStr], "Hello World");
  printf("✓ append: works with NXString objects\n");

  // Test append with NXConstantString
  NXString *appendBase2 = [NXString stringWithFormat:@"Hello"];
  BOOL appendResult2 = [appendBase2 append:@" Constants"];
  test_assert(appendResult2 == YES);
  test_assert([appendBase2 length] == 15);
  test_cstrings_equal([appendBase2 cStr], "Hello Constants");
  printf("✓ append: works with NXConstantString\n");

  // Test append empty string
  NXString *appendBase3 = [NXString stringWithFormat:@"NoChange"];
  NXString *emptyAppend = [NXString stringWithCString:""];
  BOOL appendResult3 = [appendBase3 append:emptyAppend];
  test_assert(appendResult3 == YES);
  test_assert([appendBase3 length] == 8);
  test_cstrings_equal([appendBase3 cStr], "NoChange");
  printf("✓ append: handles empty string correctly\n");

  // Test append to empty string
  NXString *appendBase4 = [NXString stringWithFormat:@""];
  NXString *appendOther4 = [NXString stringWithCString:"FirstContent"];
  BOOL appendResult4 = [appendBase4 append:appendOther4];
  test_assert(appendResult4 == YES);
  test_assert([appendBase4 length] == 12);
  test_cstrings_equal([appendBase4 cStr], "FirstContent");
  printf("✓ append: works when appending to empty string\n");

  // Test multiple appends
  NXString *appendBase5 = [NXString stringWithFormat:@"One"];
  BOOL appendResult5a = [appendBase5 append:@" Two"];
  BOOL appendResult5b = [appendBase5 append:@" Three"];
  NXString *fourStr = [NXString stringWithCString:" Four"];
  BOOL appendResult5c = [appendBase5 append:fourStr];
  test_assert(appendResult5a == YES);
  test_assert(appendResult5b == YES);
  test_assert(appendResult5c == YES);
  test_assert([appendBase5 length] ==
              18); // Fixed: "One Two Three Four" = 18 chars
  test_cstrings_equal([appendBase5 cStr], "One Two Three Four");
  printf("✓ append: works with multiple consecutive appends\n");

  // Test append with special characters
  NXString *appendBase6 = [NXString stringWithFormat:@"Test"];
  NXString *specialStr = [NXString stringWithCString:"!@#$%^&*()"];
  BOOL appendResult6 = [appendBase6 append:specialStr];
  test_assert(appendResult6 == YES);
  test_assert([appendBase6 length] == 14);
  test_cstrings_equal([appendBase6 cStr], "Test!@#$%^&*()");
  printf("✓ append: works with special characters\n");

  // Test append with numbers
  NXString *appendBase7 = [NXString stringWithFormat:@"Count: "];
  NXString *numberStr = [NXString stringWithFormat:@"%d", 12345];
  BOOL appendResult7 = [appendBase7 append:numberStr];
  test_assert(appendResult7 == YES);
  test_cstrings_equal([appendBase7 cStr], "Count: 12345");
  printf("✓ append: works with formatted numbers\n");

  // Test append to immutable string (should make it mutable)
  NXString *immutableBase = [NXString stringWithCString:"Immutable"];
  NXString *appendToImmutable = [NXString stringWithCString:" Converted"];
  BOOL immutableResult = [immutableBase append:appendToImmutable];
  test_assert(immutableResult == YES);
  test_cstrings_equal([immutableBase cStr], "Immutable Converted");
  printf("✓ append: converts immutable strings to mutable\n");

  // Test append with long strings
  NXString *longBase = [NXString
      stringWithFormat:
          @"This is a very long string that should test memory allocation"];
  NXString *longAppend =
      [NXString stringWithCString:" and this is an even longer appendage that "
                                  "will require additional memory allocation"];
  BOOL longResult = [longBase append:longAppend];
  test_assert(longResult == YES);
  test_assert([longBase length] == 145);
  printf("✓ append: works with long strings requiring reallocation\n");

  printf("\nTest 13: Testing appendCString: method...\n");

  // Test basic appendCString
  NXString *cstrBase1 = [NXString stringWithFormat:@"Hello"];
  BOOL cstrResult1 = [cstrBase1 appendCString:" C-String"];
  test_assert(cstrResult1 == YES);
  test_assert([cstrBase1 length] == 14);
  test_cstrings_equal([cstrBase1 cStr], "Hello C-String");
  printf("✓ appendCString: works with basic C-strings\n");

  // Test appendCString with empty string
  NXString *cstrBase2 = [NXString stringWithFormat:@"NoChange"];
  BOOL cstrResult2 = [cstrBase2 appendCString:""];
  test_assert(cstrResult2 == YES);
  test_assert([cstrBase2 length] == 8);
  test_cstrings_equal([cstrBase2 cStr], "NoChange");
  printf("✓ appendCString: handles empty C-string correctly\n");

  // Test appendCString to empty string
  NXString *cstrBase3 = [NXString stringWithFormat:@""];
  BOOL cstrResult3 = [cstrBase3 appendCString:"FirstContent"];
  test_assert(cstrResult3 == YES);
  test_assert([cstrBase3 length] == 12);
  test_cstrings_equal([cstrBase3 cStr], "FirstContent");
  printf("✓ appendCString: works when appending to empty string\n");

  // Test multiple appendCString calls
  NXString *cstrBase4 = [NXString stringWithFormat:@"Start"];
  BOOL cstrResult4a = [cstrBase4 appendCString:" Middle"];
  BOOL cstrResult4b = [cstrBase4 appendCString:" End"];
  test_assert(cstrResult4a == YES);
  test_assert(cstrResult4b == YES);
  test_assert([cstrBase4 length] == 16);
  test_cstrings_equal([cstrBase4 cStr], "Start Middle End");
  printf("✓ appendCString: works with multiple consecutive appends\n");

  // Test appendCString with special characters
  NXString *cstrBase5 = [NXString stringWithFormat:@"Symbols"];
  BOOL cstrResult5 = [cstrBase5 appendCString:": !@#$%^&*()"];
  test_assert(cstrResult5 == YES);
  test_assert([cstrBase5 length] == 19);
  test_cstrings_equal([cstrBase5 cStr], "Symbols: !@#$%^&*()");
  printf("✓ appendCString: works with special characters\n");

  // Test appendCString with numbers and mixed content
  NXString *cstrBase6 = [NXString stringWithFormat:@"Result"];
  BOOL cstrResult6 = [cstrBase6 appendCString:": 123ABC!@#"];
  test_assert(cstrResult6 == YES);
  test_cstrings_equal([cstrBase6 cStr], "Result: 123ABC!@#");
  printf("✓ appendCString: works with mixed numbers and characters\n");

  // Test appendCString to immutable string
  NXString *immutableCstr = [NXString stringWithCString:"Immutable"];
  BOOL immutableCstrResult = [immutableCstr appendCString:" CString"];
  test_assert(immutableCstrResult == YES);
  test_cstrings_equal([immutableCstr cStr], "Immutable CString");
  printf("✓ appendCString: converts immutable strings to mutable\n");

  // Test appendCString with long C-string
  NXString *longCstrBase = [NXString stringWithFormat:@"Base"];
  const char *longCstr = " This is a very long C-string that will test memory "
                         "allocation and reallocation capabilities";
  BOOL longCstrResult = [longCstrBase appendCString:longCstr];
  test_assert(longCstrResult == YES);
  test_assert([longCstrBase length] == 96);
  printf("✓ appendCString: works with long C-strings requiring reallocation\n");

  printf("\nTest 14: Testing appendStringWithFormat: method...\n");

  // Test basic appendStringWithFormat with string interpolation
  NXString *formatBase1 = [NXString stringWithFormat:@"Hello"];
  BOOL formatResult1 = [formatBase1 appendStringWithFormat:@" %s", "World"];
  test_assert(formatResult1 == YES);
  test_assert([formatBase1 length] == 11);
  test_cstrings_equal([formatBase1 cStr], "Hello World");
  printf("✓ appendStringWithFormat: works with basic string formatting\n");

  // Test appendStringWithFormat with numbers
  NXString *formatBase2 = [NXString stringWithFormat:@"Count: "];
  BOOL formatResult2 = [formatBase2 appendStringWithFormat:@"%d", 42];
  test_assert(formatResult2 == YES);
  test_assert([formatBase2 length] == 9);
  test_cstrings_equal([formatBase2 cStr], "Count: 42");
  printf("✓ appendStringWithFormat: works with number formatting\n");

  // Test appendStringWithFormat with multiple format specifiers
  NXString *formatBase3 = [NXString stringWithFormat:@"Data: "];
  BOOL formatResult3 = [formatBase3
      appendStringWithFormat:@"Name=%s, ID=%d, Score=%d", "Alice", 123, 95];
  test_assert(formatResult3 == YES);
  test_assert([formatBase3 length] == 34);
  test_cstrings_equal([formatBase3 cStr], "Data: Name=Alice, ID=123, Score=95");
  printf("✓ appendStringWithFormat: works with multiple format specifiers\n");

  // Test appendStringWithFormat with empty format result
  NXString *formatBase4 = [NXString stringWithFormat:@"NoChange"];
  BOOL formatResult4 = [formatBase4 appendStringWithFormat:@""];
  test_assert(formatResult4 == YES);
  test_assert([formatBase4 length] == 8);
  test_cstrings_equal([formatBase4 cStr], "NoChange");
  printf("✓ appendStringWithFormat: handles empty format string correctly\n");

  // Test appendStringWithFormat to empty string
  NXString *formatBase5 = [NXString stringWithFormat:@""];
  BOOL formatResult5 =
      [formatBase5 appendStringWithFormat:@"First: %s", "Content"];
  test_assert(formatResult5 == YES);
  test_assert([formatBase5 length] == 14);
  test_cstrings_equal([formatBase5 cStr], "First: Content");
  printf("✓ appendStringWithFormat: works when appending to empty string\n");

  // Test multiple appendStringWithFormat calls
  NXString *formatBase6 = [NXString stringWithFormat:@"Start"];
  BOOL formatResult6a = [formatBase6 appendStringWithFormat:@" %s", "Middle"];
  BOOL formatResult6b = [formatBase6 appendStringWithFormat:@" %s", "End"];
  test_assert(formatResult6a == YES);
  test_assert(formatResult6b == YES);
  test_assert([formatBase6 length] == 16);
  test_cstrings_equal([formatBase6 cStr], "Start Middle End");
  printf("✓ appendStringWithFormat: works with multiple consecutive appends\n");

  // Test appendStringWithFormat with special characters in format
  NXString *formatBase7 = [NXString stringWithFormat:@"Special"];
  BOOL formatResult7 =
      [formatBase7 appendStringWithFormat:@": %s", "!@#$%^&*()"];
  test_assert(formatResult7 == YES);
  test_assert([formatBase7 length] == 19);
  test_cstrings_equal([formatBase7 cStr], "Special: !@#$%^&*()");
  printf("✓ appendStringWithFormat: works with special characters\n");

  // Test appendStringWithFormat with mixed types
  NXString *formatBase8 = [NXString stringWithFormat:@"Mixed"];
  BOOL formatResult8 = [formatBase8
      appendStringWithFormat:@": str=%s, int=%d, char=%c", "test", 789, 'X'];
  test_assert(formatResult8 == YES);
  test_cstrings_equal([formatBase8 cStr], "Mixed: str=test, int=789, char=X");
  printf("✓ appendStringWithFormat: works with mixed format types\n");

  // Test appendStringWithFormat to immutable string
  NXString *immutableFormat = [NXString stringWithCString:"Immutable"];
  BOOL immutableFormatResult =
      [immutableFormat appendStringWithFormat:@" %s", "Formatted"];
  test_assert(immutableFormatResult == YES);
  test_cstrings_equal([immutableFormat cStr], "Immutable Formatted");
  printf("✓ appendStringWithFormat: converts immutable strings to mutable\n");

  // Test appendStringWithFormat with long formatted content
  NXString *longFormatBase = [NXString stringWithFormat:@"Base"];
  BOOL longFormatResult = [longFormatBase
      appendStringWithFormat:@" %s %d %s",
                             "This is a very long formatted string that will "
                             "test memory allocation",
                             12345, "and reallocation capabilities"];
  test_assert(longFormatResult == YES);
  test_assert([longFormatBase length] == 110);
  printf("✓ appendStringWithFormat: works with long formatted strings "
         "requiring reallocation\n");

  // Test appendStringWithFormat with hex and other number formats
  NXString *formatBase9 = [NXString stringWithFormat:@"Numbers"];
  BOOL formatResult9 = [formatBase9
      appendStringWithFormat:@": dec=%d, hex=0x%x, oct=%o", 255, 255, 255];
  test_assert(formatResult9 == YES);
  test_cstrings_equal([formatBase9 cStr],
                      "Numbers: dec=255, hex=0xff, oct=377");
  printf("✓ appendStringWithFormat: works with different number formats\n");

  // Test combining appendStringWithFormat with other append methods
  NXString *combinedBase = [NXString stringWithFormat:@"A"];
  [combinedBase append:[NXString stringWithCString:"B"]];
  [combinedBase appendCString:"C"];
  [combinedBase appendStringWithFormat:@"%s", "D"];
  [combinedBase appendStringWithFormat:@"%d", 5];
  test_cstrings_equal([combinedBase cStr], "ABCD5");
  printf(
      "✓ appendStringWithFormat: can be combined with other append methods\n");

  printf(
      "\nTest 15: Testing append methods edge cases and error conditions...\n");

  // Test append with self (should work correctly)
  NXString *selfAppend = [NXString stringWithFormat:@"Self"];
  BOOL selfResult = [selfAppend append:selfAppend];
  test_assert(selfResult == YES);
  test_cstrings_equal([selfAppend cStr], "SelfSelf");
  printf("✓ append: works when appending string to itself\n");

  // Test capacity growth with append
  NXString *capacityTest = [NXString stringWithCapacity:10];
  [capacityTest appendCString:"12345"]; // Should fit in initial capacity
  test_assert([capacityTest capacity] >= 10);
  [capacityTest
      appendStringWithFormat:@"%s", "67890ABCDEF"]; // Should require expansion
  test_assert([capacityTest capacity] > 10);
  test_cstrings_equal([capacityTest cStr], "1234567890ABCDEF");
  printf("✓ append methods properly handle capacity expansion\n");

  // Test alternating append methods
  NXString *alternating = [NXString stringWithFormat:@"A"];
  [alternating append:[NXString stringWithCString:"B"]];
  [alternating appendCString:"C"];
  [alternating append:@"D"];
  [alternating appendCString:"E"];
  [alternating appendStringWithFormat:@"%s", "F"];
  test_cstrings_equal([alternating cStr], "ABCDEF");
  printf("✓ append:, appendCString:, and appendStringWithFormat: can be used "
         "together\n");

  // Test very small strings
  NXString *tiny1 = [NXString stringWithFormat:@"A"];
  NXString *tiny2 = [NXString stringWithCString:"B"];
  [tiny1 append:tiny2];
  test_cstrings_equal([tiny1 cStr], "AB");
  printf("✓ append: works with very small strings\n");

  // Test single character appends
  NXString *singleCharAppend = [NXString stringWithFormat:@""];
  [singleCharAppend appendCString:"a"];
  [singleCharAppend appendCString:"b"];
  [singleCharAppend appendCString:"c"];
  test_cstrings_equal([singleCharAppend cStr], "abc");
  printf("✓ appendCString: works with single character appends\n");

  // Test unicode/extended ASCII characters (if supported)
  NXString *unicodeBase = [NXString stringWithFormat:@"Unicode"];
  [unicodeBase appendCString:" Test: àáâãäå"];
  test_assert([unicodeBase length] >
              13); // Length should include extended chars
  printf("✓ appendCString: handles extended ASCII characters\n");

  // Test null terminator handling
  NXString *nullTermTest = [NXString stringWithFormat:@"Before"];
  [nullTermTest appendCString:" After"];
  // Verify null terminator is properly placed
  const char *cstr = [nullTermTest cStr];
  test_assert(cstr[[nullTermTest length]] == '\0');
  printf("✓ append methods properly maintain null termination\n");

  printf("\nTest 16: Testing append methods with various string types...\n");

  // Test append with strings created by different methods
  NXString *formatBase = [NXString stringWithFormat:@"Format"];
  NXString *cstrCreated = [NXString stringWithCString:" CStr"];
  NXString *stringCreated = [NXString stringWithString:@" String"];
  NXString *capacityCreated = [NXString stringWithCapacity:20];
  [capacityCreated appendCString:" Capacity"];

  [formatBase append:cstrCreated];
  [formatBase append:stringCreated];
  [formatBase append:capacityCreated];
  test_cstrings_equal([formatBase cStr], "Format CStr String Capacity");
  printf("✓ append: works with strings created by different methods\n");

  // Test with strings that have different internal states
  NXString *referenced =
      [NXString stringWithCString:"Referenced"];          // _data = NULL
  NXString *allocated = [NXString stringWithCapacity:50]; // _data != NULL
  [allocated appendCString:"Allocated"];

  [referenced append:allocated];
  test_cstrings_equal([referenced cStr], "ReferencedAllocated");
  printf("✓ append: works between referenced and allocated strings\n");

  printf("\nTest 17: Testing trimWhitespace method...\n");

  // Test basic trimming - leading and trailing spaces
  NXString *trimTest1 = [NXString stringWithFormat:@"  Hello World  "];
  BOOL trimResult1 = [trimTest1 trimWhitespace];
  test_assert(trimResult1 == YES);
  test_assert([trimTest1 length] == 11);
  test_cstrings_equal([trimTest1 cStr], "Hello World");
  printf("✓ trimWhitespace: removes leading and trailing spaces\n");

  // Test trimming only leading spaces
  NXString *trimTest2 = [NXString stringWithFormat:@"   Leading"];
  BOOL trimResult2 = [trimTest2 trimWhitespace];
  test_assert(trimResult2 == YES);
  test_assert([trimTest2 length] == 7);
  test_cstrings_equal([trimTest2 cStr], "Leading");
  printf("✓ trimWhitespace: removes only leading spaces\n");

  // Test trimming only trailing spaces
  NXString *trimTest3 = [NXString stringWithFormat:@"Trailing   "];
  BOOL trimResult3 = [trimTest3 trimWhitespace];
  test_assert(trimResult3 == YES);
  test_assert([trimTest3 length] == 8);
  test_cstrings_equal([trimTest3 cStr], "Trailing");
  printf("✓ trimWhitespace: removes only trailing spaces\n");

  // Test trimming mixed whitespace (spaces, tabs, newlines)
  NXString *trimTest4 = [NXString stringWithFormat:@"\t\n  Mixed  \r\n\t"];
  BOOL trimResult4 = [trimTest4 trimWhitespace];
  test_assert(trimResult4 == YES);
  test_assert([trimTest4 length] == 5);
  test_cstrings_equal([trimTest4 cStr], "Mixed");
  printf("✓ trimWhitespace: removes mixed whitespace characters\n");

  // Test no trimming needed - already trimmed
  NXString *trimTest5 = [NXString stringWithFormat:@"NoTrimNeeded"];
  BOOL trimResult5 = [trimTest5 trimWhitespace];
  test_assert(trimResult5 == NO);
  test_assert([trimTest5 length] == 12);
  test_cstrings_equal([trimTest5 cStr], "NoTrimNeeded");
  printf("✓ trimWhitespace: returns NO when no trimming needed\n");

  // Test empty string
  NXString *trimTest6 = [NXString stringWithFormat:@""];
  BOOL trimResult6 = [trimTest6 trimWhitespace];
  test_assert(trimResult6 == NO);
  test_assert([trimTest6 length] == 0);
  test_cstrings_equal([trimTest6 cStr], "");
  printf("✓ trimWhitespace: handles empty string correctly\n");

  // Test string with only whitespace
  NXString *trimTest7 = [NXString stringWithFormat:@"   \t\n\r   "];
  BOOL trimResult7 = [trimTest7 trimWhitespace];
  test_assert(trimResult7 == YES);
  test_assert([trimTest7 length] == 0);
  test_cstrings_equal([trimTest7 cStr], "");
  printf("✓ trimWhitespace: handles whitespace-only string correctly\n");

  // Test single character with whitespace
  NXString *trimTest8 = [NXString stringWithFormat:@"  A  "];
  BOOL trimResult8 = [trimTest8 trimWhitespace];
  test_assert(trimResult8 == YES);
  test_assert([trimTest8 length] == 1);
  test_cstrings_equal([trimTest8 cStr], "A");
  printf("✓ trimWhitespace: handles single character correctly\n");

  // Test string with internal whitespace (should preserve it)
  NXString *trimTest9 = [NXString stringWithFormat:@"  Hello   World  "];
  BOOL trimResult9 = [trimTest9 trimWhitespace];
  test_assert(trimResult9 == YES);
  test_assert([trimTest9 length] == 13);
  test_cstrings_equal([trimTest9 cStr], "Hello   World");
  printf("✓ trimWhitespace: preserves internal whitespace\n");

  // Test trimming on immutable string (should convert to mutable)
  NXString *immutableTrim = [NXString stringWithCString:"  Immutable  "];
  BOOL immutableTrimResult = [immutableTrim trimWhitespace];
  test_assert(immutableTrimResult == YES);
  test_cstrings_equal([immutableTrim cStr], "Immutable");
  printf("✓ trimWhitespace: converts immutable strings to mutable\n");

  // Test multiple consecutive trims (second should return NO)
  NXString *multiTrim = [NXString stringWithFormat:@"  Multiple  "];
  BOOL multiTrimResult1 = [multiTrim trimWhitespace];
  BOOL multiTrimResult2 = [multiTrim trimWhitespace];
  test_assert(multiTrimResult1 == YES);
  test_assert(multiTrimResult2 == NO);
  test_cstrings_equal([multiTrim cStr], "Multiple");
  printf("✓ trimWhitespace: multiple calls work correctly\n");

  // Test very long string with lots of whitespace
  NXString *longWhitespace =
      [NXString stringWithFormat:@"                    Very Long String With "
                                 @"Lots Of Whitespace                    "];
  BOOL longWhitespaceResult = [longWhitespace trimWhitespace];
  test_assert(longWhitespaceResult == YES);
  test_cstrings_equal([longWhitespace cStr],
                      "Very Long String With Lots Of Whitespace");
  printf("✓ trimWhitespace: handles long strings with extensive whitespace\n");

  printf("\n✅ All append method tests passed!\n");
  printf("✅ All trimWhitespace tests passed!\n");

  printf("\nTest 18: Testing containsString: method...\n");

  // Test basic substring containment
  NXString *baseString = [NXString stringWithCString:"Hello World Programming"];

  // Test 1: Basic substring at beginning
  test_assert(
      [baseString containsString:[NXString stringWithCString:"Hello"]] == YES);
  printf("✓ containsString: finds substring at beginning\n");

  // Test 2: Basic substring in middle
  test_assert(
      [baseString containsString:[NXString stringWithCString:"World"]] == YES);
  printf("✓ containsString: finds substring in middle\n");

  // Test 3: Basic substring at end
  test_assert(
      [baseString containsString:[NXString stringWithCString:"Programming"]] ==
      YES);
  printf("✓ containsString: finds substring at end\n");

  // Test 4: Substring not present
  test_assert(
      [baseString containsString:[NXString stringWithCString:"Python"]] == NO);
  printf("✓ containsString: correctly returns NO for non-existent substring\n");

  // Test 5: Empty substring (should always return YES)
  test_assert([baseString containsString:[NXString stringWithCString:""]] ==
              YES);
  printf("✓ containsString: returns YES for empty substring\n");

  // Test 6: Same string
  test_assert([baseString containsString:baseString] == YES);
  printf("✓ containsString: returns YES for identical string\n");

  // Test 7: Longer substring than the string
  test_assert(
      [baseString
          containsString:[NXString stringWithCString:
                                       "Hello World Programming Extended"]] ==
      NO);
  printf("✓ containsString: returns NO for substring longer than string\n");

  // Test 8: Case sensitivity
  test_assert(
      [baseString containsString:[NXString stringWithCString:"hello"]] == NO);
  printf("✓ containsString: is case sensitive\n");

  // Test 9: Single character substring
  test_assert([baseString containsString:[NXString stringWithCString:"W"]] ==
              YES);
  printf("✓ containsString: finds single character substring\n");

  // Test 10: Empty string contains empty substring
  NXString *emptyString = [NXString stringWithCString:""];
  test_assert([emptyString containsString:[NXString stringWithCString:""]] ==
              YES);
  printf("✓ containsString: empty string contains empty substring\n");

  // Test 11: Empty string does not contain non-empty substring
  test_assert(
      [emptyString containsString:[NXString stringWithCString:"test"]] == NO);
  printf(
      "✓ containsString: empty string does not contain non-empty substring\n");

  // Test 12: Partial word matches
  test_assert([baseString containsString:[NXString stringWithCString:"ell"]] ==
              YES);
  printf("✓ containsString: finds partial word matches\n");

  printf("\n✅ All containsString tests passed!\n");

  printf("\nTest 19: Testing trimPrefix:suffix: method...\n");

  // Test 1: Basic prefix trimming
  NXString *prefixTest1 = [NXString stringWithFormat:@"prefixHello World"];
  BOOL prefixResult1 =
      [prefixTest1 trimPrefix:[NXString stringWithCString:"prefix"] suffix:nil];
  test_assert(prefixResult1 == YES);
  test_cstrings_equal([prefixTest1 cStr], "Hello World");
  printf("✓ trimPrefix:suffix: trims prefix correctly\n");

  // Test 2: Basic suffix trimming
  NXString *suffixTest1 = [NXString stringWithFormat:@"Hello Worldsuffix"];
  BOOL suffixResult1 =
      [suffixTest1 trimPrefix:nil suffix:[NXString stringWithCString:"suffix"]];
  test_assert(suffixResult1 == YES);
  test_cstrings_equal([suffixTest1 cStr], "Hello World");
  printf("✓ trimPrefix:suffix: trims suffix correctly\n");

  // Test 3: Both prefix and suffix trimming
  NXString *bothTest1 = [NXString stringWithFormat:@"prefixHello Worldsuffix"];
  BOOL bothResult1 =
      [bothTest1 trimPrefix:[NXString stringWithCString:"prefix"]
                     suffix:[NXString stringWithCString:"suffix"]];
  test_assert(bothResult1 == YES);
  test_cstrings_equal([bothTest1 cStr], "Hello World");
  printf("✓ trimPrefix:suffix: trims both prefix and suffix correctly\n");

  // Test 4: No matching prefix
  NXString *noMatchPrefix = [NXString stringWithFormat:@"Hello World"];
  BOOL noMatchPrefixResult =
      [noMatchPrefix trimPrefix:[NXString stringWithCString:"nomatch"]
                         suffix:nil];
  test_assert(noMatchPrefixResult == NO);
  test_cstrings_equal([noMatchPrefix cStr], "Hello World");
  printf("✓ trimPrefix:suffix: returns NO when prefix doesn't match\n");

  // Test 5: No matching suffix
  NXString *noMatchSuffix = [NXString stringWithFormat:@"Hello World"];
  BOOL noMatchSuffixResult =
      [noMatchSuffix trimPrefix:nil
                         suffix:[NXString stringWithCString:"nomatch"]];
  test_assert(noMatchSuffixResult == NO);
  test_cstrings_equal([noMatchSuffix cStr], "Hello World");
  printf("✓ trimPrefix:suffix: returns NO when suffix doesn't match\n");

  // Test 6: Empty prefix and suffix
  NXString *emptyTrimTest = [NXString stringWithFormat:@"Hello World"];
  BOOL emptyTrimResult =
      [emptyTrimTest trimPrefix:[NXString stringWithCString:""]
                         suffix:[NXString stringWithCString:""]];
  test_assert(emptyTrimResult == NO);
  test_cstrings_equal([emptyTrimTest cStr], "Hello World");
  printf("✓ trimPrefix:suffix: handles empty prefix/suffix correctly\n");

  // Test 7: Prefix longer than string
  NXString *longPrefixTest = [NXString stringWithFormat:@"Hi"];
  BOOL longPrefixResult =
      [longPrefixTest trimPrefix:[NXString stringWithCString:"VeryLongPrefix"]
                          suffix:nil];
  test_assert(longPrefixResult == NO);
  test_cstrings_equal([longPrefixTest cStr], "Hi");
  printf("✓ trimPrefix:suffix: handles prefix longer than string\n");

  // Test 8: Suffix longer than string
  NXString *longSuffixTest = [NXString stringWithFormat:@"Hi"];
  BOOL longSuffixResult =
      [longSuffixTest trimPrefix:nil
                          suffix:[NXString stringWithCString:"VeryLongSuffix"]];
  test_assert(longSuffixResult == NO);
  test_cstrings_equal([longSuffixTest cStr], "Hi");
  printf("✓ trimPrefix:suffix: handles suffix longer than string\n");

  // Test 9: Exact match (entire string is prefix)
  NXString *exactPrefixTest = [NXString stringWithFormat:@"exact"];
  BOOL exactPrefixResult =
      [exactPrefixTest trimPrefix:[NXString stringWithCString:"exact"]
                           suffix:nil];
  test_assert(exactPrefixResult == YES);
  test_cstrings_equal([exactPrefixTest cStr], "");
  printf("✓ trimPrefix:suffix: handles exact prefix match (empty result)\n");

  // Test 10: Exact match (entire string is suffix)
  NXString *exactSuffixTest = [NXString stringWithFormat:@"exact"];
  BOOL exactSuffixResult =
      [exactSuffixTest trimPrefix:nil
                           suffix:[NXString stringWithCString:"exact"]];
  test_assert(exactSuffixResult == YES);
  test_cstrings_equal([exactSuffixTest cStr], "");
  printf("✓ trimPrefix:suffix: handles exact suffix match (empty result)\n");

  // Test 11: Overlapping prefix and suffix (entire string)
  NXString *overlapTest = [NXString stringWithFormat:@"test"];
  BOOL overlapResult =
      [overlapTest trimPrefix:[NXString stringWithCString:"te"]
                       suffix:[NXString stringWithCString:"st"]];
  test_assert(overlapResult == YES);
  test_cstrings_equal([overlapTest cStr], "");
  printf("✓ trimPrefix:suffix: handles overlapping prefix/suffix correctly\n");

  // Test 12: Empty string input
  NXString *emptyInputTest = [NXString stringWithFormat:@""];
  BOOL emptyInputResult =
      [emptyInputTest trimPrefix:[NXString stringWithCString:"prefix"]
                          suffix:[NXString stringWithCString:"suffix"]];
  test_assert(emptyInputResult == NO);
  test_cstrings_equal([emptyInputTest cStr], "");
  printf("✓ trimPrefix:suffix: handles empty input string correctly\n");

  // Test 13: Case sensitivity
  NXString *caseTest = [NXString stringWithFormat:@"PrefixHello WorldSuffix"];
  BOOL caseResult = [caseTest trimPrefix:[NXString stringWithCString:"prefix"]
                                  suffix:[NXString stringWithCString:"suffix"]];
  test_assert(caseResult == NO);
  test_cstrings_equal([caseTest cStr], "PrefixHello WorldSuffix");
  printf("✓ trimPrefix:suffix: is case sensitive\n");

  // Test 14: Single character prefix and suffix
  NXString *singleCharTest = [NXString stringWithFormat:@"aHello Worldz"];
  BOOL singleCharResult =
      [singleCharTest trimPrefix:[NXString stringWithCString:"a"]
                          suffix:[NXString stringWithCString:"z"]];
  test_assert(singleCharResult == YES);
  test_cstrings_equal([singleCharTest cStr], "Hello World");
  printf("✓ trimPrefix:suffix: handles single character prefix/suffix\n");

  // Test 15: Multiple consecutive trims
  NXString *multiTrimTest = [NXString stringWithFormat:@"abcHello Worldxyz"];
  BOOL multiTrim1 = [multiTrimTest trimPrefix:[NXString stringWithCString:"a"]
                                       suffix:[NXString stringWithCString:"z"]];
  BOOL multiTrim2 = [multiTrimTest trimPrefix:[NXString stringWithCString:"b"]
                                       suffix:[NXString stringWithCString:"y"]];
  BOOL multiTrim3 = [multiTrimTest trimPrefix:[NXString stringWithCString:"c"]
                                       suffix:[NXString stringWithCString:"x"]];
  test_assert(multiTrim1 == YES);
  test_assert(multiTrim2 == YES);
  test_assert(multiTrim3 == YES);
  test_cstrings_equal([multiTrimTest cStr], "Hello World");
  printf("✓ trimPrefix:suffix: works with multiple consecutive trims\n");

  printf("\n✅ All trimPrefix:suffix tests passed!\n");

  return 0;
}
