#include "Test.h"
#include <objc/objc.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

#ifdef SYSTEM_NAME_PICO
// HACK
void *stdout = NULL;
void *stderr = NULL;
#endif

int test_runtime_04(void);

int main(void) { return TestMain("test_runtime_04", test_runtime_04); }

int test_runtime_04() {
  // Allocate a test object with a value
  Test *test = [[Test alloc] initWithValue:42];
  test_assert(test != NULL);

  // Check the value
  test_assert([test value] == 42);

  // Set the value to 100
  [test setValue:100];
  test_assert([test value] == 100);

  // Return success
  return 0;
}
