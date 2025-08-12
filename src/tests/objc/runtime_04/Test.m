#include "Test04.h"
#include <objc/objc.h>
#include <tests/tests.h>

int test_runtime_04() {
  // Allocate a test object with a value
  Test04 *test = [[Test04 alloc] initWithValue:42];
  test_assert(test != NULL);

  // Check the value
  test_assert([test value] == 42);

  // Set the value to 100
  [test setValue:100];
  test_assert([test value] == 100);

  // Return success
  return 0;
}
