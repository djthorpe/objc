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
  NXLog(@"Testing NXLog method");
  NXLog(@"This is a test log message with a number: %d", 42);
  NXLog(@"Another message with a string: %@", @"Hello, World!");

  // If the logs are printed correctly, the test passes
  return 0; // Indicating success
}
