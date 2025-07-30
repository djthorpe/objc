#include <NXFoundation/NXFoundation.h>
#include <runtime-sys/memory.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

int test_log_methods(void);

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) {
  NXZone *zone = [NXZone zoneWithSize:2048];
  test_assert(zone != nil);
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];
  test_assert(pool != nil);

  // Run the test for log methods
  int returnValue = TestMain("NXFoundation_23", test_log_methods);

  // Clean up
  [pool release];
  [zone release];

  // Return the result of the test
  return returnValue;
}

///////////////////////////////////////////////////////////////////////////////
// TESTS

int test_log_methods(void) {
  size_t result;

  // Test 1: Basic string logging
  result = NXLog(@"Testing NXLog method");
  test_assert(result == 20); // "Testing NXLog method" = 20 characters

  // Test 2: Format specifier with number
  result = NXLog(@"Number: %d", 42);
  test_assert(result == 10); // "Number: 42" = 10 characters

  // Test 3: Object format specifier with string constant
  result = NXLog(@"String: %@", @"Hello");
  test_assert(result == 13); // "String: Hello" = 13 characters

  // Test 4: Object format specifier with nil
  result = NXLog(@"Nil object: %@", nil);
  test_assert(result == 17); // "Nil object: <nil>" = 17 characters

  // Test 5: Multiple %@ in one format string
  result = NXLog(@"%@ and %@", @"First", @"Second");
  test_assert(result == 16); // "First and Second" = 16 characters

  // Test 6: Mixed format specifiers
  result = NXLog(@"Mixed: %d, %@, %s", 123, @"Object", "CString");
  test_assert(result == 27); // "Mixed: 123, Object, CString" = 27 characters

  // Test 7: Create an NXString object and test its description
  NXString *str = [[NXString alloc] initWithString:@"NXString Object"];
  test_assert(str != nil);
  result = NXLog(@"NXString: %@", str);
  // "NXString: NXString Object" = 25 characters
  test_assert(result == 25);
  [str release];

  // Test 8: Create an NXNumber and test its description
  NXNumber *num = [NXNumber numberWithInt32:999];
  test_assert(num != nil);
  result = NXLog(@"Number obj: %@", num);
  // "Number obj: 999" = 15 characters
  test_assert(result == 15);

  // Test 9: Test with an object that doesn't conform to
  // NXConstantStringProtocol
  NXZone *zone = [NXZone zoneWithSize:1024];
  test_assert(zone != nil);
  result = NXLog(@"Zone: %@", zone);
  // "Zone: NXZone" = 12 characters (since zone description might not conform)
  test_assert(result == 12);
  [zone release];

  // Test 10: Empty string object
  result = NXLog(@"Empty: %@", @"");
  test_assert(result == 7); // "Empty: " = 7 characters

  // Test 11: Long format string to test buffer handling
  result = NXLog(
      @"This is a very long format string with an object %@ in the middle",
      @"TEST");
  test_assert(result == 67); // Total length should be 67 characters

  // Test 12: Multiple objects with different lengths
  result = NXLog(@"A:%@, B:%@, C:%@", @"X", @"YZ", @"ABCD");
  test_assert(result == 17); // "A:X, B:YZ, C:ABCD" = 17 characters

  // Test 13: Time interval formatting with %t
  result = NXLog(@"Duration: %t", 5 * Second + 250 * Millisecond);
  test_assert(result == 18); // "Duration: 5s 250ms" = 18 characters

  // Test 14: Zero time interval
  result = NXLog(@"Zero: %t", (NXTimeInterval)0);
  test_assert(result == 8); // "Zero: 0s" = 8 characters

  // Test 15: Mixed format with time interval
  result = NXLog(@"Test %d took %t", 42, 1 * Second + 500 * Millisecond);
  test_assert(result == 21); // "Test 42 took 1s 500ms" = 21 characters

  return 0; // Indicating success
}
