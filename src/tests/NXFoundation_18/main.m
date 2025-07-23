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

  printf("\nTest 7: Testing memory management...\n");

  // Test autoreleased strings don't crash
  for (int i = 0; i < 5; i++) {
    NXString *tempStr = [NXString stringWithCString:"Temporary"];
    test_assert(tempStr != nil);
    test_assert([tempStr length] == 9);
  }
  printf("✓ Memory management works correctly\n");

  // Clean up manually allocated strings
  [str2 release];
  [str4 release];
  [formatted2 release];
  [emptyStr2 release];

  printf("\n✅ All NXString method tests passed!\n");
  return 0;
}
