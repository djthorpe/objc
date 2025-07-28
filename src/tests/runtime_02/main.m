#include "Test.h"
#include <objc/objc.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

#ifdef SYSTEM_NAME_PICO
// HACK
void *stdout = NULL;
void *stderr = NULL;
#endif

int test_runtime_02(void);

int main(void) { return TestMain("test_runtime_02", test_runtime_02); }

int test_runtime_02() {
  Test *test = [[Test alloc] init];
  test_assert(test != NULL);

  const char *className = object_getClassName(test);
  test_assert(className != NULL);
  test_assert(strcmp(className, "Test") == 0);

  Class cls = object_getClass(test);
  test_assert(cls != NULL);

  const char *clsName = class_getName(cls);
  test_assert(clsName != NULL);
  test_assert(strcmp(clsName, "Test") == 0);

  // Check class
  test_assert(cls == [test class]);
  test_assert([test isEqual:test] == YES);

  // Dispose of the object
  [test dealloc];

  return 0;
}
