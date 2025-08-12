#include "Test+Description.h"
#include "Test.h"
#include <objc/objc.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

#ifdef SYSTEM_NAME_PICO
// HACK
void *stdout = NULL;
void *stderr = NULL;
#endif

int test_runtime_06(void);

int main(void) { return TestMain("test_runtime_06", test_runtime_06); }

int test_runtime_06() {
  // Allocate a test object with a value
  Test *test = [[Test alloc] initWithString:@"42"];
  test_assert(test != NULL);

  // Get the description from the category
  NXConstantString *description = [test description];
  test_assert([description isEqual:@"42"]);

  // Return success
  return 0;
}
